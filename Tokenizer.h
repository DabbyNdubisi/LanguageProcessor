#ifndef TOKENIZER_H
#define TOKENIZER_H
/*
 * author: Dabby Ndubisi
 * A c++ tokenizer that reads from the brown corpus
 * and normalizes the texts based on particular
 * parameters
 */
#include "Dependencies.h"

class Tokenizer
{
    string START = "-START-";
    string END = "-END-";

public:
    Tokenizer();

    /*!
     *     @desc: tokenizes a string into a vector of sentences
     *   @params: in stringToTokenize: string&
     *  @returns: sentences: vector<string>
     */
    vector<string> tokenizeSentence(string&);

    /*!
     *     @desc: tokenizes a string into a vector of words
     *   @params: in stringToTokenize: string&
     *  @returns: words: vector<string>
     */
    vector<string> tokenizeWord(string&);

    /*!
     *     @desc: extracts the given tag from a "word/tag" string
     *            pattern.
     *   @params: in word: string& (word to find the tag for)
     *  @returns: word-Tag: tuple<string, string>
     */
    tuple<string, string> extractWordTag(string&);
    
    tuple<string, int> extractWordGoldHead(string&);

    // Helper Functions
    /*!
     *     @desc: trim white space from start
     *   @params: in word: string& (word to trim)
     *  @returns: trimmedWord: string&
     */
    string& ltrim(string&);

    /*!
     *     @desc: trim white space from end
     *   @params: in word: string& (word to trim)
     *  @returns: trimmedWord: string&
     */
    string& rtrim(string&);

    /*!
     *     @desc: trim white space from both ends
     *   @params: in word: string& (word to trim)
     *  @returns: trimmedWord: string&
     */
    string& trim(string&);

    /*!
     *     @desc: split a given word into components based
     *            a given delimeter
     *   @params: in word: string& (word to split)
     *            in delimeter: char
     *  @returns: splitWords: vector<string>
     */
    vector<string> split(string&, char);

    /*!
     *     @desc: normalizes a string by replacing particular patterns
     *   @params: in word: string& (word to normalize)
     *  @returns: normalizedWord: string
     */
    string normalize(string&);
    
    /*!
     *  @desc: finds contractions and shortforms in given word and expands it
     *         to a longer parse friendly version
     *  @params: in word: string& (word to expand)
     *  @returns: vector of new words. (will contain only one element if word didn't have a contraction)
     */
    vector<string> expandWordContraction(string&);
    
    
    
    vector<string> wordsBrownToPenn(vector<string>&);
    
    
    /*!
     *  @desc: tokenizes the sentence input from the user 
     *         to match the expected input for our parser and tagger.
     *  @params: (in) sentence: string& (sentence to tokenize)
     *  @returns: vector of words, parsed according to what is expected by our syntax parser and POS tagger.
     */
    vector<string> tokenizeUserSentenceInput(string);
};
#endif
