#ifndef SCFL_H
#define SCFL_H

#include <vector>
#include <string>
#include "scfg_gnf.h"
#include <map>

class StochasticLanguage {
  StochasticGrammar* gr;
  std::vector<std::string> vocabulary;
public:
  StochasticLanguage(StochasticGrammar* gr_, std::initializer_list<std::string> v={});
  unsigned addWordToVocabulary(std::string s);
  std::vector<std::vector<std::string>> generateSomeText(unsigned sentanceNum);
  std::vector<std::string> generateSomeSentaces(unsigned sentanceNum, bool withSpaces=true);
  void speakSomeText(unsigned sentanceNum);
  std::map<std::string, unsigned> countFrequencies(unsigned sentanceNum);
};

#endif
