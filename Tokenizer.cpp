#include <fstream>
#include <sstream>
#include <iostream>
#include "Tokenizer.h"

using namespace std;

Tokenizer::Tokenizer()
{

}

string Tokenizer::normalize(string word)
{
  return "";
}

vector<string> Tokenizer::tokenizeSentence(string str)
{
  return split(str, '\n');
}

vector<string> Tokenizer::tokenizeWord(string sentence)
{
  return split(sentence, ' ');
}

tuple<string, string> Tokenizer::extractWordTag(string word)
{
  auto wordTagVector = split(word, '/');
  if (wordTagVector.size() == 2)
  {
    return tuple<string, string>(trim(wordTagVector[0]), wordTagVector[1]);
  }
  else
  {
    return tuple<string, string>(word, "error");
  }

}

vector<string> Tokenizer::split(string &str,  char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
    internal.push_back(tok);
  }

  return internal;
}

// trim from start
inline string& Tokenizer::ltrim(string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
inline string& Tokenizer::rtrim(string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
inline string& Tokenizer::trim(string &s) {
        return ltrim(rtrim(s));
}
