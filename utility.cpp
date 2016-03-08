#include "utility.h"
#include "Tokenizer.h"
#include <fstream>
#include <sstream>
#include <tuple>

vector<Category> getCorpusCategories()
{
  auto categoryList = vector<Category>();

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

  return categoryList;
}


string readCorpus(string location)
{
  auto categories = getCorpusCategories();
  auto count = categories.size();
  location = (location.compare("") == 0) ? "Corpus/" : location;
  std::ostringstream stream;

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


// assume each sentence in corpus has the "\n" at the end
vector<tuple<string, string>> getTrainingData()
{
  auto corpus = readCorpus("Corpus/");
  auto examples = vector<tuple<string, string>>();
  Tokenizer tokenizer;
  auto sentences = tokenizer.tokenizeSentence(corpus);
  for (auto sentence : sentences)
  {
    auto words = tokenizer.tokenizeWord(sentence);
    for (auto word : words)
    {
      examples.push_back(tokenizer.extractWordTag(word));
    }
  }

  return examples;
}
