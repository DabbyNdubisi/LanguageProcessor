//
//  Parse.cpp
//  LanguageProcessor
//
//  Created by Dabby Ndubisi on 2016-04-10.
//  Copyright © 2016 Dabby Ndubisi. All rights reserved.
//

#include "Parse.h"

Parse::Parse(int& numWords)
{
    this->numWords = numWords;
    
    for(int i = 0; i <= numWords; i++)
    {
        heads.push_back(-1);
        lefts.push_back(vector<int>());
        rights.push_back(vector<int>());
    }
    heads.pop_back();
}

void Parse::addDependency(int& headIndex, int& childIndex)
{
    heads[childIndex] = headIndex;
    
    if(childIndex < headIndex)
        lefts[headIndex].push_back(childIndex);
    else
        rights[headIndex].push_back(childIndex);
}