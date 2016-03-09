#include "POSTagger.h"
#include <iostream>
#include <fstream>
using namespace std;

int main()
{
  ofstream out("output.txt");
  POSTagger tagger;
  auto examples = vector<tuple<string, string>>();
  tagger.getTrainingData(examples);
  for (auto i = 0; i < examples.size(); i++)
  {
      out << get<0>(examples[i]) << " : " << get<1>(examples[i]) << endl;
  }
}
