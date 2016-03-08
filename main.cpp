#include "utility.h"
#include <iostream>
#include <fstream>
using namespace std;

int main()
{
  ofstream out("output.txt");
  vector<tuple<string, string>> examples = getTrainingData();

  for (auto i = 0; i < examples.size(); i++)
  {
      out << get<0>(examples[i]) << " : " << get<1>(examples[i]) << endl;
  }
}
