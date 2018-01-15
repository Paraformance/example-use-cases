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

