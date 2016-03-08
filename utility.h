#ifndef UTILITY_H
#define UTILITY_H
/*
 * author: Dabby Ndubisi
 * Utility functions for various purposes related to the Parser
 */
#include <vector>
#include <string>
#include <tuple>
using namespace std;

/* Data structure for the differente category types in
 * the Brown corpus and their start and end numbers
 */
struct Category
{
  string name;
  string prefixTag;
  int start;
  int end;

  Category(string n, string p, int s, int e):
  name(n), prefixTag(p), start(s), end(e)
  {}
};

/*
 * DEFAULT: Brown corpus
 * Corupus Syntax: word/POS
 * ca01 - ca44 news
 * cb01 - cb27 editorial
 * cc01 - cc17 reviews
 * cd01 - cd17 religion
 * ce01 - ce36 hobbies
 * cf01 - cf48 lores
 * cg01 - cg75 belles_lettres
 * ch01 - ch30 government
 * cj01 - cj80 learned
 * ck01 - ck29 fiction
 * cl01 - cl24 mystery
 * cm01 - cm06 science_fiction
 * cn01 - cn29 adventure
 * cp01 - cp29 romance
 * cr01 - cr09 humor
 */

/*!
 *    @desc: return an array of possible categories in the Brown Corpus
 *  @params: none
 *  @return: categoryArray: Category*
 */
vector<Category> getCorpusCategories();

/*!
 *        @desc: reads all contents of the Brown corpus into a string
 *      @params: in location: String (location of the corpus)
 *      @return: corpusContent: String
 */
string readCorpus(string location);


vector<tuple<string, string>> getTrainingData();

#endif
