//
//  DPPerceptron.cpp
//  LanguageProcessor
//
//  Created by Dabby Ndubisi on 2016-04-10.
//  Copyright © 2016 Dabby Ndubisi. All rights reserved.
//

#include "DPPerceptron.h"
#include <regex>

DPPerceptron::DPPerceptron()
{
    
}

map<Transition, double> DPPerceptron::score(map<string, int>& features)
{
    //  Find dot product of weight vector and features to produce
    //  score for particular class
    map<Transition, double> scores;
    scores[SHIFT] = 0;
    scores[LEFT] = 0;
    scores[RIGHT] = 0;
    
    for (auto const& feature : features)
    {
        if (featureClassWeightMap.find(feature.first) != featureClassWeightMap.end())
        {
            auto classWeightsForFeature = featureClassWeightMap[feature.first];
            
            for (auto const& classWeight : classWeightsForFeature)
                scores[classWeight.first] += classWeight.second * feature.second;
            
        }
    }
    
    return scores;
}

void DPPerceptron::updateFeatureWeights(Transition& truth, Transition& guess, map<string, int>& features)
{
    if (truth == guess)
        return;
    
    for (auto const& feature : features)
    {
        featureClassWeightMap[feature.first][guess] -= 1;
        featureClassWeightMap[feature.first][truth] += 1;
    }
}

void DPPerceptron::setClasses(set<Transition>& copy)
{
    classes.insert(copy.begin(), copy.end());
}

set<Transition>& DPPerceptron::getClasses()
{
    return classes;
}

istream& operator>>(istream& in, DPPerceptron& perceptron)
{
    char feat_class_weight[256];
    regex e("(.+) : (.+) : (.+)");
    smatch sm2;
    int count = 0;
    while(in.getline(feat_class_weight, 256) && regex_match(string(feat_class_weight), sm2, e))
    {
        Transition t = (Transition)stoi(sm2[2].str());
        perceptron.featureClassWeightMap[sm2[1].str()][t] = stod(sm2[3].str());
        count++;
    }
    return in;
}

ostream& operator<<(ostream& out, DPPerceptron& perceptron)
{
    for(auto const& featureMapPair : perceptron.featureClassWeightMap)
    {
        for(auto const& classWeightPair : featureMapPair.second)
        {
            out << featureMapPair.first << " : " << classWeightPair.first << " : " << classWeightPair.second << endl;
        }
    }
    
    return out;
}