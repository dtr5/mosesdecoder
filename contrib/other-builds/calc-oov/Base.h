#pragma once
#include <fstream>

class Base
{
public:
  virtual ~Base() {}
  virtual void CreateVocab(std::ifstream &corpusStrme) = 0;
  virtual void CalcOOV(std::ifstream &testStrme, std::ofstream *oovStream) const = 0;
  

};

