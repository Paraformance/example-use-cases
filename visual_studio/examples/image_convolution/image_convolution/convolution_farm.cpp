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

/****** Intel TBB specific part ******/
class CPU_Convolution_Farm_Component {
private: 
  unsigned short **images;
  unsigned short **masks;
  unsigned short **outputs;
public:
  void operator()(const blocked_range<size_t>& r) const {
    for (size_t i=r.begin(); i!=r.end(); i++) {
      outputs[i] = new unsigned short[dim*dim];
        process_image(images[i], masks[i], outputs[i]);
    }
  }
  
  CPU_Convolution_Farm_Component (unsigned short **images,
                                  unsigned short **masks,
                                  unsigned int nr_images)
    : images(images), masks(masks) {
    outputs = new unsigned short *[nr_images];
  }
};
/************************************/

int main(int argc, char * argv[]) {
  unsigned int nr_images, pattern, do_chunking, min_chunk_size=1;
  
  if (argc<4) {
    std::cerr << "use: " << argv[0] << " <nr_cpu_workers> <imageSize> <nrImages> [<chunking>]\n";
    return 1;
  }

  nr_cpu_w = atoi(argv[1]);
  dim = atoi(argv[2]);
  nr_images = atoi(argv[3]);
  if (argc>4)
    do_chunking = atoi(argv[4]);
  else
    do_chunking = 0;
  
  images = new unsigned short *[nr_images];
  masks = new unsigned short *[nr_images];
  out_images = new unsigned short *[nr_images];
  for (int i=0; i<nr_images; i++)
    out_images[i] = new unsigned short[dim*dim];

  
  /***** Intel TBB specific part *********/
  task_scheduler_init init(nr_cpu_w);
    /***************************************/
  
  if (do_chunking)
    min_chunk_size = nr_images / nr_cpu_w;

  double beginning = get_current_time();
  char image_name[255];
  for (int i=0; i<nr_images; i++) {
    get_image_name(i, image_name);
    read_image_and_mask(image_name, &(images[i]), &(masks[i]));
  }                          

  /******* Intel TBB farm ***************/
  parallel_for(blocked_range<size_t>(0, nr_images, min_chunk_size), CPU_Convolution_Farm_Component(images, masks, nr_images));
  /**************************************/
  double end = get_current_time();
  cout << "Runtime with " << nr_cpu_w << " farm workers is " << end - beginning << endl;
  return 0;
}




