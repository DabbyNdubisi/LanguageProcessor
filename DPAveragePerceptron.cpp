//
//  DPAveragePerceptron.cpp
//  LanguageProcessor
//
//  Created by Dabby Ndubisi on 2016-04-10.
//  Copyright © 2016 Dabby Ndubisi. All rights reserved.
//

#include "DPAveragePerceptron.h"

DPAveragePerceptron::DPAveragePerceptron()
{
    classes.insert(SHIFT);
    classes.insert(LEFT);
    classes.insert(RIGHT);
}

void DPAveragePerceptron::updateFeatureWeights(Transition& truth, Transition& guess, map<string, int>&features)
{
    if (truth == guess)
        return;
    
    auto update = [this] (Transition clas, string feature, double value)
    {
        auto numIterationsUnchanged = this->currentIteration - this->timeStamps[feature][clas];
        this->accumulatedWeights[feature][clas] += numIterationsUnchanged * this->featureClassWeightMap[feature][clas];
        this->featureClassWeightMap[feature][clas] += value;
        this->timeStamps[feature][clas] = this->currentIteration;
    };
    
    currentIteration++;
    for (auto const& feature : features)
    {
        update(truth, feature.first, 1.0);
        update(guess, feature.first, -1.0);
    }
}


void DPAveragePerceptron::averageWeights()
{
    for (auto const& featureClassWeights : featureClassWeightMap)
    {
        map<Transition, double> newClassWeights;
        
        for (auto const& classWeightsForFeature : featureClassWeights.second)
        {
            double total = accumulatedWeights[featureClassWeights.first][classWeightsForFeature.first];
            total += (currentIteration - timeStamps[featureClassWeights.first][classWeightsForFeature.first]) * classWeightsForFeature.second;
            double averagedWeight = total / double(currentIteration);
            if (averagedWeight > 0)
                newClassWeights[classWeightsForFeature.first] = averagedWeight;
        }
        
        featureClassWeightMap[featureClassWeights.first] = newClassWeights;
    }
}
