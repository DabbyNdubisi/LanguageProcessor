#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>
#include <map>
#include <regex>
#include "Tokenizer.h"

using namespace std;

map<string, vector<string>> knownContractions = {
    {"shouldda", vector<string>({"should", "da"})},
    {"lookit", vector<string>({"look", "at"})},
    {"whaddya", vector<string>({"whad", "d", "ya"})},
    {"gonna", vector<string>({"gon", "na"})},
    {"gotta", vector<string>({"got", "ta"})},
    {"wanta", vector<string>({"wan", "ta"})},
    {"hafta", vector<string>({"haf", "ta"})},
    {"oughta", vector<string>({"ough", "ta"})},
    {"lemme", vector<string>({"lem", "me"})},
    {"wanna", vector<string>({"wan", "na"})},
    {"buncha", vector<string>({"bunch", "a"})},
    {"musta", vector<string>({"must", "a"})},
    {"howda", vector<string>({"how", "da"})},
    {"coulda", vector<string>({"could", "a"})},
    {"ahm", vector<string>({"Ah", "m"})},
    {"gimme", vector<string>({"gim", "me"})},
    {"woulda", vector<string>({"would", "a"})},
    {"outta", vector<string>({"out", "ta"})},
    {"willya", vector<string>({"will", "ya"})},
    
};

Tokenizer::Tokenizer()
{

}

vector<string> Tokenizer::tokenizeUserSentenceInput(string input)
{
    vector<string> words;
    //TODO: write tokenization code to make input match training input
    return words;
}

vector<string> Tokenizer::tokenizeSentence(string& str)
{
  return split(str, '\n');
}

vector<string> Tokenizer::tokenizeWord(string& sentence)
{
    return split(sentence, ' ');
}

vector<string> Tokenizer::wordsBrownToPenn(vector<string>& brawnWords)
{
    vector<string> allWords;
    for(auto& rawWord : brawnWords)
    {
        transform(rawWord.begin(), rawWord.end(), rawWord.begin(), ::tolower);
        
        auto cleanWords = expandWordContraction(rawWord);
        allWords.insert( allWords.end(), cleanWords.begin(), cleanWords.end() );
    }
    
    return allWords;
}

string Tokenizer::normalize(string& word)
{
  // All words must be in lower case
  transform(word.begin(), word.end(), word.begin(), ::tolower);

  /* Cases to consider:
   * i)   Hashtags
   * ii)   Numbers from 1800 - 2100 => !YEAR
   * iii)  Other Digits
   * iv)  ***Add more cases as we go***
   */
  regex eHashTag("^#\\w+$");
  regex eYear("^(18|21)\\d{2}$");
  regex eDigit("^\\d+$");
  if (regex_match(word, eHashTag))
    return "!HASHTAG";
  else if(regex_match(word, eYear))
    return "!YEAR";
  else if(regex_match(word, eDigit))
    return "!DIGITS";
  else
    return word;
}

tuple<string, string> Tokenizer::extractWordTag(string& word)
{
  regex e("\\s*(.+/?.*)/(.+)");
  smatch sm;
  if(regex_search(word, sm, e))
  {
    auto str = sm[1].str();
    return tuple<string, string>(normalize(str), sm[2]);
  }
  else
  {
    return tuple<string, string>(word, "");
  }
}

tuple<string, int> Tokenizer::extractWordGoldHead(string& rawWord)
{
    regex e("\\s*(.+)\t(.+)\t(.+)");
    smatch sm;
    if(regex_search(rawWord, sm, e))
    {
        auto str = sm[1].str();
        return tuple<string, int>(normalize(str), stoi(sm[3].str()));
    }
    else
    {
        return tuple<string, int>(rawWord, -2);
    }
}

vector<string> Tokenizer::expandWordContraction(string& wordTag)
{
    auto word_tag = extractWordTag(wordTag);
    vector<string> expandedWords;
    
    regex eNegator("(.+)(\\*)");
    regex eDoubleContraction("(.+)\\+(.+)\\+(.+)");
    regex eContraction("(.+)\\+(.+)");
    regex ePossession("([\\w]+)(\\$)");
    
    string tag = get<1>(word_tag);
    string word = get<0>(word_tag);
    smatch sm;
    
    if (regex_search(tag, sm, eNegator))
    {
        //  Found negator, so word is one of the
        //  negative verb contractions
        //  ..n't/..* => ../.. and n't/*
        
        size_t found = word.find("n't");
        if(found != string::npos)
        {
            auto sub1 = word.substr(0, found);
            expandedWords.push_back(sub1 + string("/") + sm[1].str());
            expandedWords.push_back(string("n't") + string("/") + sm[2].str());
        }
        else
        {
            size_t found1 = word.find("not");
            if(found1 != string::npos)
            {
                auto sub1 = word.substr(0, found1);
                expandedWords.push_back(sub1 + string("/") + sm[1].str());
                expandedWords.push_back(string("not") + string("/") + sm[2].str());
            }
            else
            {
                expandedWords.push_back(wordTag);
            }
        }
    }
    else if(regex_search(tag, sm, eDoubleContraction))
    {
        //  Found a double word contraction.
        //  ..'../..+..+.. => ../.. , ../.. and ../..
        
        auto wordVect = knownContractions[word];
        
        if(wordVect.size() > 0)
        {
            for(int i = 0; i < wordVect.size(); i++)
                expandedWords.push_back(wordVect[i] + string("/") + sm[i+1].str());
        }
        else
        {
            expandedWords.push_back(wordTag);
        }
        
    }
    else if(regex_search(tag, sm, eContraction))
    {
        //  Found a word contraction.
        //  ..'../..+.. => ../.. and '../..
        size_t found = word.find("'");
        if(found != string::npos)
        {
            auto sub1 = word.substr(0, found);
            auto sub2 = word.substr(found, string::npos);
            
            expandedWords.push_back(sub1 + string("/") + sm[1].str());
            expandedWords.push_back(sub2 + string("/") + sm[2].str());
        }
        else
        {
            auto wordVect = knownContractions[word];
            
            if(wordVect.size() > 0)
            {
                for(int i = 0; i < wordVect.size(); i++)
                    expandedWords.push_back(wordVect[i] + string("/") + sm[i+1].str());
            }
            else
            {
                expandedWords.push_back(wordTag);
            }
        }
    }
    else if(regex_search(tag, sm, ePossession))
    {
        //  Found a possessive word of the form
        //  ..'s or ..s'
        
        size_t found = word.find("'");
        if(found != string::npos)
        {
            auto sub1 = word.substr(0, found);
            auto sub2 = word.substr(found, string::npos);
            
            expandedWords.push_back(sub1 + string("/") + sm[1].str());
            expandedWords.push_back(sub2 + string("/") + sm[2].str());
        }
        else
        {
            expandedWords.push_back(wordTag);
        }
    }
    else
        expandedWords.push_back(wordTag);
    
    return expandedWords;
}

vector<string> Tokenizer::split(string &str,  char delimiter) {
  vector<string> internal;
  stringstream ss(str); // Turn the string into a stream.
  string tok;

  while(getline(ss, tok, delimiter)) {
      if(tok.compare("") != 0 || tok.compare("\t") != 0)
          internal.push_back(tok);
  }

  return internal;
}

// trim from start
string& Tokenizer::ltrim(string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
string& Tokenizer::rtrim(string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
string& Tokenizer::trim(string &s) {
        return ltrim(rtrim(s));
}
