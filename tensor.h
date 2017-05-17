#ifndef TENSOR_H
#define TENSOR_H

#include <initializer_list>
#include <cstdlib>
#include <cstring>
#include "blob.h"
#include <iostream>
#include "ring_buffer.h"

template<class T>
struct Tensor {
  Blob size;
  Blob mem;
  Tensor() {}

  Tensor(std::initializer_list<unsigned> dims) {
    size = Blob(dims.size() * sizeof(unsigned));
    memcpy(size.bytes, dims.begin(), sizeof(unsigned)*dims.size());
    unsigned s=1;
    for (unsigned k: dims)
      s *= k;
    mem = Blob(s*sizeof(T));
  }

  Tensor(unsigned dim_size, unsigned* dims) {
    size = Blob(dim_size * sizeof(unsigned));
    memcpy(size.bytes, dims, sizeof(unsigned)*dim_size);
    unsigned s=1;
    for (unsigned k=0; k<dim_size; k++)
      s *= dims[k];
    mem = Blob(s*sizeof(T));
  }

  T getElement(std::initializer_list<unsigned> pos) {
    unsigned index = 0;
    unsigned s = mem.getSize() / sizeof(T);
    for (unsigned i=0; i < pos.size(); i++) {
      //std::cout << ((unsigned*)(size.bytes))[i] << " " << std::flush;
      s /= ((unsigned*)(size.bytes))[i];

      index += s * ((unsigned*)(pos.begin()))[i];
    }
    return ((T*)(mem.bytes))[index];
  }

  void setElement(std::initializer_list<unsigned> pos, T element) {
    unsigned index = 0;
    unsigned s = mem.getSize() / sizeof(T);
    for (unsigned i=0; i < pos.size(); i++) {
      s /= ((unsigned*)(size.bytes))[i];
      index += s * ((unsigned*)(pos.begin()))[i];
    }
    (T*)(mem.bytes)[index] = element;
  }

  T* getSlice(std::initializer_list<unsigned> pos) {
    unsigned index = 0;
    unsigned s = mem.getSize() / sizeof(T);
    for (unsigned i=0; i < pos.size(); i++) {
      s /= ((unsigned*)(size.bytes))[i];

      index += s * ((unsigned*)(pos.begin()))[i];
    }
    return (T*)(mem.bytes) + index;
  }

  template<int N>
  T* getSlice(RingBuffer<unsigned, N>& buf) {
    unsigned index = 0;
    unsigned s = mem.getSize() / sizeof(T);
    for (unsigned i=0; i < N; i++) {
      s /= ((unsigned*)(size.bytes))[i];

      index += s * buf[i+1];
    }
    return (T*)(mem.bytes) + index;
  }

  T* getSliceForHMM2S(RingBuffer<unsigned, 2>& buf, unsigned stackTop) {
    unsigned index = 0;
    unsigned s = mem.getSize() / sizeof(T);
    for (unsigned i=0; i < 2; i++) {
      s /= ((unsigned*)(size.bytes))[i];
      index += s * buf[i+1];
    }

    s /= ((unsigned*)(size.bytes))[2];
    index += s * stackTop;

    return (T*)(mem.bytes) + index;
  }


};

#endif
