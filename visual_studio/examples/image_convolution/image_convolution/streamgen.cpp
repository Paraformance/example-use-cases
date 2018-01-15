void * StreamGen::svc(void * task) {
  for(int i=0; i<NIMGS; i++) {
    ff_send_out((void *) images[i]);
  }
  return NULL;
}

