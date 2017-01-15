#ifndef POSAVERAGE_PERCEPTRON_H
#define POSAVERAGE_PERCEPTRON_H

#include "POSPerceptron.h"

//TODO: comment for class

class POSAveragePerceptron: public POSPerceptron
{

    map<string, map<Tag, double>> accumulatedWeights;
    map<string, map<Tag, double>> timeStamps;
    int currentIteration;

public:
    POSAveragePerceptron();
    void updateFeatureWeights(Tag&, Tag&, map<string, int>&);
    void averageWeights();
};

#endif
