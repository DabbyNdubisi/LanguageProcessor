#include "POSTagger.h"
#include "Tokenizer.h"
#include "errorCodes.h"

int POSTagger::getCorpusCategories(vector<Category>& categoryList)
{
  categoryList.push_back(Category("news", "ca", 1, 44));
  categoryList.push_back(Category("editorial", "cb", 1, 27));
  categoryList.push_back(Category("reviews", "cc", 1, 17));
  categoryList.push_back(Category("religion", "cd", 1, 17));
  categoryList.push_back(Category("hobbies", "ce", 1, 36));
  categoryList.push_back(Category("lores", "cf", 1, 48));
  categoryList.push_back(Category("belles_lettres", "cg", 1, 75));
  categoryList.push_back(Category("government", "ch", 1, 30));
  categoryList.push_back(Category("learned", "cj", 1, 80));
  categoryList.push_back(Category("fiction", "ck", 1, 29));
  categoryList.push_back(Category("mystery", "cl", 1, 24));
  categoryList.push_back(Category("science_fiction", "cm", 1, 6));
  categoryList.push_back(Category("adventure", "cn", 1, 29));
  categoryList.push_back(Category("romance", "cp", 1, 29));
  categoryList.push_back(Category("humor", "cr", 1, 9));

  return SUCCESS;
}


string POSTagger::readCorpus(string& location)
{
  auto categories = vector<Category>();

  if(getCorpusCategories(categories))
  {
    auto count = categories.size();
    location = (location.compare("") == 0) ? "Corpus/" : location;
    stringstream stream;

    for (auto category : categories)
    {
      for(auto j = 1; j <= category.end; j++)
      {
        auto fileNumber =  (j < 10) ? "0" + to_string(j) : to_string(j);
        string fileString = location + category.prefixTag + fileNumber;
        ifstream in(fileString);
        if (in)
        {
          stream << in.rdbuf();
          in.close();
        }
      }
    }
    return (stream.str());
  }
  return "";
}


// assume each sentence in corpus has the "\n" at the end
int POSTagger::getTrainingData(vector<tuple<string, string>>& trainingData)
{
  string corpusLocation = "Corpus/";
  auto corpus = readCorpus(corpusLocation);

  if(corpus.compare("") == 0)
  {
    return FAILURE;
  }

  Tokenizer tokenizer;
  auto sentences = tokenizer.tokenizeSentence(corpus);
  for (auto sentence : sentences)
  {
    auto words = tokenizer.tokenizeWord(sentence);
    for (auto word : words)
    {
      word = tokenizer.trim(word);
      if(word.compare("") != 0)
      {
        trainingData.push_back(tokenizer.extractWordTag(word));
      }
    }
  }

  return SUCCESS;
}
