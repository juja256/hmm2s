#ifndef SCFL_H
#define SCFL_H

#include <vector>
#include <string>
#include "scfg_gnf.h"

class StochasticLanguage {
  StochasticGrammar* gr;
  std::vector<std::string> vocabulary;
public:
  StochasticLanguage(StochasticGrammar* gr_, std::initializer_list<std::string> v={});
  unsigned addWordToVocabulary(std::string s);
  std::vector<std::vector<std::string>> generateSomeText(unsigned sentanceNum);
  void speakSomeText(unsigned sentanceNum);
};

#endif
