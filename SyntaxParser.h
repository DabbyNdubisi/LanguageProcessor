//
//  SyntaxParser.hpp
//  LanguageProcessor
//
//  Created by Dabby Ndubisi on 2016-04-03.
//  Copyright © 2016 Dabby Ndubisi. All rights reserved.
//

#ifndef SYNTAXPARSER_H
#define SYNTAXPARSER_H

#include "Dependencies.h"
#include "errorCodes.h"
#include "Parse.h"
#include "DPAveragePerceptron.h"
#include "POSTagger.h"

/* A Greedy Parser based on the Shift-Reduce algorithm */

class SyntacticParser
{
    
    //  Instance Variables
    shared_ptr<set<Transition>> classes;
    unique_ptr<DPAveragePerceptron> brain;
    unique_ptr<POSTagger> tagger;
    
    /*!
     *         @desc   executes the specified Parse transition on the stack and the
     *                 next element in the buffer (if applicable)
     *       @params   move: (in) Transition (move to apply)
     *                  idx: (in) int (currentIndex in our buffer)
     *                stack: (inout) vector<int>& (shift-reduce parse stack of word indices)
     *                parse: (out) Parse& (parse result object)
     *      @returns  index: int (current Parse index in buffer)
     */
    int transition(Transition, int, vector<int>&, Parse&);
    
    
    /*!
     *         @desc    return a list of all the valid moves based on the current state of
     *                  our stack, and buffer
     *       @params        index: (in) int (current index in buffer)
     *                   buffSize: (in) int (size of the buffer)
     *                  stackSize: (in) int (size of the stack)
     *      @returns    validMoves: vector<Transition>
     */
    vector<Transition> validMoves(int, int, int);
    
    //TODO: comment
    map<string, int> getFeaturesForCurrentState(vector<string>&, vector<string>, int&, int&, vector<int>&, Parse&);
    
    
    
    //TODO: comment
    string readCorpus(string&);
    
    //TODO: comment
    int train();
    
    //TODO: Comment
    int trainOne(int iteration, vector<string>& words, vector<int>& goldHeads);
    
    int loadTrainData();
    
    int saveTrainData();
    
    //TODO: Comment
    vector<Transition> getGoldMoves(int index, int bufferSize, vector<int>& stack, vector<int>& predictions, vector<int>& goldHeads);
    
    //TODO: Comment
    int explore(Transition guess, Transition truth, int iteration, int index, vector<int>& stack, Parse& dep);
    
    //TODO: Comment
    int trainTenFold();
    
    //TODO: Comment
    int trainOneOut();
    
public:
    SyntacticParser(bool=true);
    
    //TODO: comment
    tuple<vector<string>, Parse> parse(vector<string>&);
};

#endif /* SyntaxParser_hpp */
