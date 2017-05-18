#include "hmm.h"
#include <memory>
#include <cstdlib>
#include <iostream>
#include "settings.h"

unsigned sample_from_categorical(unsigned n, double* pr) {
  std::unique_ptr<double[]> a(new double[n+1]);
  a[0] = 0;
  #ifdef DEBUG
  std::cout << "Sampling from CDF: " << a[0] << " ";
  #endif
  for (unsigned i = 0; i < n; i++) {
    a[i+1] = a[i] + pr[i];
    #ifdef DEBUG
    std::cout << a[i+1] << " ";
    #endif
  }
  #ifdef DEBUG
  std::cout << "\n";
  #endif

  double r = a[n] * ((double)rand() / RAND_MAX);
  #ifdef DEBUG
  std::cout << "Random: " << r << "\n";
  #endif
  if (r < a[n-1] * 0.5) {
    for (unsigned i=1; i<=n; i++) {
      if (r > a[i-1] && r < a[i]) {
        return i-1;
      }
    }
  }
  else {
    for (int i=n; i>=1; i--) {
      if (r > a[i-1] && r < a[i])
        return i-1;
    }
  }
}

/* M - state space size */
/* L - observation space size */
/* E - order by emissions */
template<int E>
HMM<E>::HMM(unsigned M_, unsigned L_):
  M(M_), L(L_), takts(0) {
    this->transitionPr = Tensor<double>({this->M, this->M});
    unsigned obs_dims[E+1];
    for (unsigned i=0; i<E; i++)
      obs_dims[i] = M;
    obs_dims[E] = L;
    this->observationPr = Tensor<double>(E+1, obs_dims);
    this->startPr = Tensor<double>({this->M});
}

template<int E>
HMM<E>::HMM(Tensor<double> t, Tensor<double> o, Tensor<double> start):
  M( ((unsigned*)(start.size.bytes))[0] ), L( ((unsigned*)(o.size.bytes))[o.size.getSize()-1] ), takts(0) {
  observationPr = o;
  startPr = start;
  transitionPr = t;
}

template<int E>
unsigned HMM<E>::step() {
  this->cur_state_buf.writeTop(
    sample_from_categorical(M, this->transitionPr.getSlice({this->cur_state_buf[0]}))
  );
  this->takts++;

  if (this->takts >= E && this->cur_state_buf[0] != EMPTY_STATE)
    this->cur_observation = sample_from_categorical( this->L, this->observationPr.template getSlice<E>(this->cur_state_buf) );
  else if (this->takts < E)
    this->cur_observation = NO_OBSERVATION;
  else
    this->cur_observation = HMM_HALT;

  #ifdef DEBUG
  std::cout << "Stepping HMM" << "(" << this->M << ", " << this->L << "). Current state: " << this->cur_state_buf[0] << "\n";
  #endif
  return this->cur_observation;
}

template<int E>
unsigned HMM<E>::start() {
  this->cur_state_buf.writeTop(
    sample_from_categorical(M, startPr.getSlice({}) )
  );
  this->takts++;

  if (this->takts >= E && this->cur_state_buf[0] != EMPTY_STATE)
    this->cur_observation = sample_from_categorical(this->L, this->observationPr.template getSlice<E>(this->cur_state_buf) );
  else if (this->takts < E)
    this->cur_observation = NO_OBSERVATION;
  else
    this->cur_observation = HMM_HALT;

  #ifdef DEBUG
  std::cout << "Starting HMM" << "(" << this->M << ", " << this->L << "). Start state: " << this->cur_state_buf[0] << "\n";
  #endif
  return this->cur_observation;
}

template<int E>
unsigned HMM<E>::run(unsigned n, unsigned* arr) {
  unsigned old_takts = this->takts;
  if (this->takts == 0) {
    arr[0] = this->start();
    /* HMM ought not to halt at the start */
  }
  else {
    arr[0] = this->step();
    if (this->cur_state_buf[0] == EMPTY_STATE)
      goto halt;
  }

  for (unsigned i=1; i<n; i++) {
    arr[i] = this->step();
    if (this->cur_state_buf[0] == EMPTY_STATE)
      goto halt;
  }

halt:
  return this->takts - old_takts;
}

template<int E>
std::vector<unsigned> HMM<E>::run(unsigned n) {
  std::vector<unsigned> v;
  if (this->takts == 0) {
    v.push_back(this->start());
  }
  else {
    v.push_back(this->step());
    if (v.back() == HMM_HALT)
      goto halt;
  }
  for (unsigned i=1; i<n; i++) {
    v.push_back(this->step());
    if (v.back() == HMM_HALT)
      goto halt;
  }

halt:
  return v;
}

template<int E>
std::vector<unsigned> HMM<E>::run() {
  std::vector<unsigned> v;
  if (this->takts == 0) {
    v.push_back(this->start());
  }
  else {
    v.push_back(this->step());
    if (v.back() == HMM_HALT)
      goto halt;
  }
  for (;;) {
    v.push_back(this->step());
    if (v.back() == HMM_HALT)
      goto halt;
  }

halt:
  return v;
}

template<int E>
unsigned HMM<E>::restart() {
  this->takts = 0;
  return this->start();
}

template<int E>
HMM<E>::~HMM() {

}

template class HMM<1>;
template class HMM<2>;
