#ifndef POSTAGGER_H
#define POSTAGGER_H
/*
 * author: Dabby Ndubisi
 * Provides interface for retrieving the POS tag
 * of  a given word. Results are dependent on the
 * kind of training used
 * Brown corpus used as training data
 */

#include "Dependencies.h"
#include "POSAveragePerceptron.h"

class POSTagger {
    friend class SyntacticParser;
    
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

    //  Instance Variables
    shared_ptr<set<Tag>> classes;
    unique_ptr<unordered_map<string, Tag>> knownTags;
    unique_ptr<POSAveragePerceptron> brain;

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
    
    /*!
     *        @desc:  creates training data as a (word, tag) tuple and populates
     *                the output parameter vector with all the (word, tag) contained
     *                in the corpus
     *      @params:  (out) trainingData: vector<tuple<string, string>>&
     *     @returns:  success or failure
     */
    int train();

    /*!
     *       @desc: Feature extraction function.
     *     @params: (in) current word position in sentence: Int
     *              (in) wordForFeatureExtraction: string&
     *              (in) context: string&
     *              (in) prevTag: string&
     *              (in) prevTag2: string&
     *    @returns: map of features, and their frequency of occurrence 
     *              for the given word based on the given context.
     */
    map<string, int> getFeaturesForWord(int, string&, vector<string>&, string&, string&);

    //TODO: comment
    void populateKnownTags(vector<string>& sentences);
    
    //TODO: comment
    int saveTrainData();
    
    //TODO: comment
    int loadTrainData();
    

public:
    //TODO: comment
    POSTagger(bool=true);
    
    //TODO: comment
    vector<Tag> tagString(string&);
    
    //TODO: comemnt
    vector<Tag> tagWords(vector<string> words);


};

#endif
