//
//  Parse.hpp
//  LanguageProcessor
//
//  Created by Dabby Ndubisi on 2016-04-10.
//  Copyright © 2016 Dabby Ndubisi. All rights reserved.
//

#ifndef PARSE_H
#define PARSE_H

#include "Dependencies.h"

/*  A datastructure for holding our parsed results */
class Parse
{
    friend class SyntacticParser;
public:   
    //  Instance variables to keep track of the dependency
    //  results from our parsing of the sentence.
    vector<int> heads;
    vector<vector<int>> lefts;
    vector<vector<int>> rights;
    int numWords;
    

    Parse(int&);
    void addDependency(int&, int&);
    
};

#endif /* PARSE_H */
