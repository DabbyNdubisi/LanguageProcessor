//
//  DPAveragePerceptron.h
//  LanguageProcessor
//
//  Created by Dabby Ndubisi on 2016-04-10.
//  Copyright © 2016 Dabby Ndubisi. All rights reserved.
//

#ifndef DPAVERAGEPERCEPTRON_H
#define DPAVERAGEPERCEPTRON_H

#include "DPPerceptron.h"

//TODO: comment for class

class DPAveragePerceptron: public DPPerceptron
{
    
    map<string, map<Transition, double>> accumulatedWeights;
    map<string, map<Transition, double>> timeStamps;
    int currentIteration;
    
public:
    DPAveragePerceptron();
    void updateFeatureWeights(Transition&, Transition&, map<string, int>&);
    void averageWeights();
};


#endif /* DPAVERAGEPERCEPTRON_H */
