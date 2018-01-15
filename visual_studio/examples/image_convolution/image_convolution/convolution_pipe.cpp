#include <png.h>
#include <tbb/tbb.h>
//#include <CL/cl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <string>
#include <fstream>
#include <sys/time.h>
#include <thread>

int dim, nr_cpu_w, nr_gpu_w;
const int mask_dim=8;

unsigned short **images;
unsigned short **masks;
unsigned short **out_images;
int nr_images;

using namespace std;
using namespace tbb;

typedef struct {
  char *image_name;
  int image_ind;
  unsigned short *image;
  unsigned short *mask;
} task_t;

double get_current_time()
{
  static int start = 0, startu = 0;
  struct timeval tval;
  double result;
  
  if (gettimeofday(&tval, NULL) == -1)
    result = -1.0;
  else if(!start) {
    start = tval.tv_sec;
    startu = tval.tv_usec;
    result = 0.0;
  }
  else
    result = (double) (tval.tv_sec - start) + 1.0e-6*(tval.tv_usec - startu);
  
  return result;
}

void process_image(unsigned short *in_image, unsigned short *mask, unsigned short *out_image) {
  int vstep = mask_dim/2;
  int hstep = mask_dim/2;
  float sumFX;
  int left,right,top,bottom,mask_index,index;
  for(int x = 0; x < dim; x++)
    for(int y = 0; y < dim; y++) {
      left    = (x           <  vstep) ? 0         : (x - vstep);
      right   = ((x + vstep - 1) >= dim) ? dim - 1 : (x + vstep - 1); 
      top     = (y           <  hstep) ? 0         : (y - hstep);
      bottom  = ((y + hstep - 1) >= dim)? dim - 1  : (y + hstep - 1); 
      sumFX = 0;
      
      for(int j = left; j <= right; j++)
        for(int k = top ; k <= bottom; k++) {
          mask_index = (k - (y - hstep)) * mask_dim  + (j - (x - vstep));
          index     = k                 * dim      + j;
          sumFX += ((float)in_image[index] * mask[mask_index]);
        }
      
      sumFX += 0.5f;
      out_image[y*dim + x] = (unsigned short) sumFX;
    }
}

ushort *read_image(const char *fileName, png_uint_32 height) {
  int i, header_size = 8, is_png;
  char header[8];
  FILE *fp = fopen(fileName,"rb");
  png_structp png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_infop info_ptr = png_create_info_struct(png_ptr), end_info = png_create_info_struct(png_ptr);
  png_bytep raw_data; 
  png_bytepp row_pointers;
  png_uint_32 row_bytes;

  fread (header, 1, header_size, fp);
  is_png = !png_sig_cmp((png_bytep)header,0,header_size);
  if (!is_png) { printf("not a png\n"); return(NULL);}
  png_init_io(png_ptr,fp);
  png_set_sig_bytes(png_ptr,header_size);
  png_read_info(png_ptr, info_ptr);
  row_bytes = png_get_rowbytes(png_ptr, info_ptr);
  raw_data = (png_bytep) png_malloc(png_ptr, height*row_bytes);
  row_pointers = (png_bytepp) png_malloc(png_ptr, height*sizeof(png_bytep));
  for (i=0; i<height; i++)
    row_pointers[i] = raw_data+i*row_bytes;
  png_set_rows(png_ptr, info_ptr, row_pointers);
  png_read_rows(png_ptr,row_pointers,NULL,height);

  return (unsigned short *)raw_data;

}

void get_image_name(int i, char *image_name_c) {
    string image_name = "images/image" + to_string(i) + ".png";
    strcpy(image_name_c, image_name.c_str());
}

void read_image_and_mask(char *image_name, unsigned short **image, unsigned short **mask) {
  *image = read_image(image_name, dim);
  *mask = new unsigned short[mask_dim*mask_dim]();
  float val = 1.0f/(mask_dim * 2.0f - 1.0f);
  unsigned short y = mask_dim/2;
  for(int j=0; j < mask_dim; j++) 
    (*mask)[y*mask_dim + j] = val;
  unsigned short x = mask_dim/2;
  for(int j=0; j < mask_dim; j++) 
    (*mask)[j*mask_dim + x] = val;
}

/******* Intel TBB Components ********/
class Read_Image_Filter: public filter {
public:  
  void *operator()(void *in_task_v) {
    task_t *in_task = (task_t *) in_task_v;
    read_image_and_mask (in_task->image_name, &(in_task->image), &(in_task->mask));
    return (void *) in_task;
  }
  
  Read_Image_Filter() : filter(parallel) {}
};

class Create_Inputs_Filter: public filter {
private:
  unsigned int nr_images;
  unsigned int image_ind;
  task_t *tasks;
public:
  void *operator()(void *) {
    if (image_ind < nr_images) {
      get_image_name(image_ind, tasks[image_ind].image_name);
      tasks[image_ind].image_ind = image_ind;
      image_ind++;
      return (void *) (&(tasks[image_ind-1]));
    }   
    else {
      return NULL;
    }
  }
  
  Create_Inputs_Filter(unsigned int nr_images) :
    nr_images(nr_images), image_ind(0), filter(serial_in_order) {
    tasks = new task_t[nr_images];
    for (int i=0; i<nr_images; i++) {
      tasks[i].image_name = new char[255];
    }
  }
};
  
class CPU_Convolution_Pipe_Component: public filter {
public:
  void *operator()(void *task_v) {
    task_t *task = (task_t *)task_v;
    unsigned short *image = task->image;
    unsigned short *mask = task->mask;
    int image_ind = task->image_ind;
    process_image(image, mask, out_images[image_ind]);
    return NULL;
  }

  CPU_Convolution_Pipe_Component (unsigned int nr_images) : filter(parallel) {}
};
/*************************************/

int main(int argc, char * argv[]) {
  
  if (argc<3) 
    std::cerr << "use: " << argv[0] << " <imageSize> <nrImages> [<chunking>]\n";
  dim = atoi(argv[1]);
  nr_images = atoi(argv[2]);
  
  images = new unsigned short *[nr_images];
  masks = new unsigned short *[nr_images];
  out_images = new unsigned short *[nr_images];
  for (int i=0; i<nr_images; i++)
    out_images[i] = new unsigned short[dim*dim];

  double beginning = get_current_time();
  /****** Intel TBB Pipe ******************/
  pipeline pipe;
  Create_Inputs_Filter stage1(nr_images);
  pipe.add_filter(stage1);
  Read_Image_Filter stage2;
  pipe.add_filter(stage2);
  CPU_Convolution_Pipe_Component stage3(nr_images);
  pipe.add_filter(stage3);
  pipe.run(nr_images);
  /***************************************/   
  double end = get_current_time();
  cout << "Runtime is " << end - beginning << endl;
  return 0;
}



