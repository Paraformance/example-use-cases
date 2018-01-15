#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include <CL/cl.h>

extern cl_int width;
extern cl_int height;
extern cl_uint maskWidth;
extern cl_uint maskHeight;
//cl_int worker_width=8;
//cl_int worker_height=8;
//cl_uint inputDimension;
//cl_uint maskDimension;
//int quiet=1;
//int nworkers;
//int nworkers2;
//int gnworkers;
//const int max_strlen=10;

//double get_current_time();

struct task_t{
  task_t(){
    outpt = new ushort[width*height];
    msk = new cl_float[maskWidth * maskHeight];
  }
  ushort *inpt;
  ushort *outpt;
  cl_float* msk;
};
#endif
