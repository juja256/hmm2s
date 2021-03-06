#ifndef HMM_H
#define HMM_H

#include <stack>
#include <vector>
#include "tensor.h"
#include "ring_buffer.h"
#include "scfg_gnf.h"

#define NO_OBSERVATION 0xFFFFFFFF
#define HMM_HALT 0xFFFFFFFE
#define EMPTY_STATE 0

unsigned sample_from_categorical(unsigned n, double* pr);

template<int E>
class HMM: public StochasticGrammar {
protected:

  unsigned M, L;
  unsigned cur_observation;
  unsigned takts;
  RingBuffer<unsigned, E> cur_state_buf;

public:
  Tensor<double> transitionPr;
  Tensor<double> observationPr;
  Tensor<double> startPr;

  HMM(unsigned M, unsigned L);
  HMM(Tensor<double> t, Tensor<double> o, Tensor<double> start);
  ~HMM();
  virtual unsigned start();
  virtual unsigned step();
  virtual unsigned restart();
  virtual unsigned run(unsigned n, unsigned* arr);
  virtual std::vector<unsigned> run(unsigned n);
  virtual std::vector<unsigned> run();
  virtual std::vector<unsigned> infer(unsigned nonterminal=START_SYMBOL);
};

#endif
