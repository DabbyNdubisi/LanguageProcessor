#ifndef TOKENIZER_H
#define TOKENIZER_H
/*
 * author: Dabby Ndubisi
 * A c++ tokenizer that reads from the brown corpus
 * and normalizes the texts based on particular
 * parameters
 */
#include <string>
#include <vector>
#include <tuple>
#include <memory>
#include <stdio.h>
#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

using namespace std;

class Tokenizer
{
  string START = "-START-";
  string END = "-END-";

  public:
    Tokenizer();

    vector<string> tokenizeSentence(string);
    vector<string> tokenizeWord(string);
    tuple<string, string> extractWordTag(string);
    // trim from start
    string& ltrim(string&);
    // trim from end
    string& rtrim(string&);
    // trim from both ends
    string& trim(string&);
    vector<string> split(string&, char);
    string normalize(string);
};
#endif
