#ifndef POSTAGGER_H
#define POSTAGGER_H
/*
 * author: Dabby Ndubisi
 * Provides interface for retrieving the POS tag
 * of  a given word. Results are dependent on the
 * kind of training used
 * Brown corpus used as training data
 */

#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <tuple>
using namespace std;

class POSTagger {
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
   *    @desc: Retrieves all possible categories in the Brown Corpus
   *  @params: categories: vector<Category>
   *  @return: categoryArray: Category*
   */
  int getCorpusCategories(vector<Category>&);

  /*!
   *         @desc: Reads all contents of the Brown corpus into a string
   *       @params: in location: String (location of the corpus)
   *      @returns: corpusContent: String
   */
  string readCorpus(string&);

public:
  /*!
   *        @desc:  creates training data as a (word, tag) tuple and populates
   *                the output parameter vector with all the (word, tag) contained
   *                in the corpus
   *      @params:  (out) trainingData: vector<tuple<string, string>>&
   *     @returns:  success or failure
   */
  int getTrainingData(vector<tuple<string, string>>&);


};

#endif
