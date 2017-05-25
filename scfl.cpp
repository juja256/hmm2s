#include "scfl.h"
#include <iostream>
#include <string>
#include "hmm.h"

StochasticLanguage::StochasticLanguage(StochasticGrammar* gr_, std::initializer_list<std::string> v): gr(gr_){
  vocabulary.push_back("#");
  vocabulary.insert(vocabulary.end(), v);
}
unsigned StochasticLanguage::addWordToVocabulary(std::string s) {
  vocabulary.push_back(s);
  return vocabulary.size();
}
std::vector<std::vector<std::string>> StochasticLanguage::generateSomeText(unsigned sentanceNum) {
  std::vector<std::vector<std::string>> text;
  for (unsigned i=0; i<sentanceNum; i++) {
    std::vector<unsigned> raw = gr->infer();
    std::vector<std::string> v;
    for (unsigned j=0; j<raw.size(); j++) {
      if (raw[j] == NO_OBSERVATION || raw[j] == HMM_HALT) continue;
      if (raw[j] > vocabulary.size()) throw StochasticGrammarException(SMALL_VOCABULARY);
      v.push_back(vocabulary[raw[j]]);
    }
    text.push_back(v);
  }
  return text;
}
void StochasticLanguage::speakSomeText(unsigned sentanceNum) {
  std::vector<std::vector<std::string>> text = this->generateSomeText(sentanceNum);
  for (unsigned i=0; i<text.size(); i++) {
    for (unsigned j=0; j<text[i].size(); j++) {
      std::cout << text[i][j] << " ";
    }
    std::cout << ". ";
  }
  std::cout << "\n";
}
