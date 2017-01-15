#ifndef POSPERCEPTRON_H
#define POSPERCEPTRON_H

#include "Dependencies.h"

//TODO: comment For class

class POSPerceptron
{
    friend ostream& operator<<(ostream&, POSPerceptron&);
    friend istream& operator>>(istream&, POSPerceptron&);
protected:
    map<string, map<string, double>> featureClassWeightMap;
    set<Tag> classes;

public:
    POSPerceptron();
    Tag predictTag(map<string, int>&);
    void updateFeatureWeights(Tag&, Tag&, map<string, int>&);

    void setClasses(set<Tag>&);
    set<Tag>& getClasses();
};

#endif
