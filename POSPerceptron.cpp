#include "POSPerceptron.h"
#include <regex>

POSPerceptron::POSPerceptron()
{

}

Tag POSPerceptron::predictTag(map<string, int>& features)
{
    //  Find dot product of weight vector and features to produce
    //  score for particular class
    map<Tag, double> scores;

    for (auto const& feature : features)
    {
        if (featureClassWeightMap.find(feature.first) != featureClassWeightMap.end())
        {
            auto classWeightsForFeature = featureClassWeightMap[feature.first];

            for (auto const& classWeight : classWeightsForFeature)
                scores[classWeight.first] += classWeight.second * feature.second;

        }
    }

    return scores.size() > 0 ? max_element(scores.begin(), scores.end(),
                              [](const pair<Tag, double>& p1, const pair<Tag, double>& p2) {
                                  return p1.second < p2.second; })->first : *(this->classes.begin());
}

void POSPerceptron::updateFeatureWeights(Tag& truth, Tag& guess, map<string, int>& features)
{
    if (truth == guess)
        return;

    for (auto const& feature : features)
    {
        featureClassWeightMap[feature.first][guess] -= 1;
        featureClassWeightMap[feature.first][truth] += 1;
    }
}

void POSPerceptron::setClasses(set<Tag>& copy)
{
    classes.insert(copy.begin(), copy.end());
}

set<Tag>& POSPerceptron::getClasses()
{
    return classes;
}

istream& operator>>(istream& in, POSPerceptron& perceptron)
{
    char feat_class_weight[256];
    regex e("(.+) : (.+) : (.+)");
    smatch sm2;
    int count = 0;
    while(in.getline(feat_class_weight, 256) && regex_match(string(feat_class_weight), sm2, e))
    {
        perceptron.featureClassWeightMap[sm2[1].str()][sm2[2].str()] = stod(sm2[3].str());
        count++;
    }
    return in;
}

ostream& operator<<(ostream& out, POSPerceptron& perceptron)
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