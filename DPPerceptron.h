//
//  DPPerceptron.h
//  LanguageProcessor
//
//  Created by Dabby Ndubisi on 2016-04-10.
//  Copyright © 2016 Dabby Ndubisi. All rights reserved.
//

#ifndef DPPERCEPTRON_H
#define DPPERCEPTRON_H

#include "Dependencies.h"

enum Transition
{
    SHIFT = 0, LEFT, RIGHT
};

class DPPerceptron
{
    friend ostream& operator<<(ostream&, DPPerceptron&);
    friend istream& operator>>(istream&, DPPerceptron&);
protected:
    map<string, map<Transition, double>> featureClassWeightMap;
    set<Transition> classes;
    
public:
    DPPerceptron();
    map<Transition, double> score(map<string, int>&);
    void updateFeatureWeights(Transition&, Transition&, map<string, int>&);
    
    void setClasses(set<Transition>&);
    set<Transition>& getClasses();
};

#endif
