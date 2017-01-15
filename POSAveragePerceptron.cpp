#include "POSAveragePerceptron.h"

POSAveragePerceptron::POSAveragePerceptron()
{
}

void POSAveragePerceptron::updateFeatureWeights(Tag& truth, Tag& guess, map<string, int>&features)
{
    if (truth.compare(guess) == 0)
        return;

    auto update = [this] (Tag clas, string feature, double value)
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


void POSAveragePerceptron::averageWeights()
{
  for (auto const& featureClassWeights : featureClassWeightMap)
  {
    map<Tag, double> newClassWeights;

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
