#include <ff/node.hpp>

class StreamGen: public ff::ff_node {
public:
  StreamGen(int NIMGS, char* images[]):NIMGS(NIMGS),images(images) {}

  void * svc(void * task) {
    for(int i=0; i<NIMGS; i++) {
      ff_send_out((void *) images[i]);
    }
    return NULL;
  }
  
protected:
  int NIMGS;
  char** images;
};
