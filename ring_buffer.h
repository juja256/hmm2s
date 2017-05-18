#ifndef RING_BUFFER_H
#define RING_BUFFER_H
#include <iostream>
static unsigned true_mod(int a, unsigned N) {
  if (a>=0) return a % N;
  else return a%N + N;
}

template<class T, int N>
struct RingBuffer {
  T buf[N];
  unsigned top;

  RingBuffer(): top(0) {}
  void writeTop(T e) {
    top = true_mod(top+1, N); // top + 1 % N;
    buf[top] = e;
  }
  T& operator[](int i) {
    return buf[true_mod(top + i , N)];
  }
};

#endif
