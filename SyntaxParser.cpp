//
//  SyntaxParser.cpp
//  LanguageProcessor
//
//  Created by Dabby Ndubisi on 2016-04-03.
//  Copyright © 2016 Dabby Ndubisi. All rights reserved.
//

#include "SyntaxParser.h"
#include "POSTagger.h"
#include "Tokenizer.h"

SyntacticParser::SyntacticParser(bool load)
{
    classes = make_shared<set<Transition>>();
    brain = unique_ptr<DPAveragePerceptron>(new DPAveragePerceptron());
    tagger = unique_ptr<POSTagger>(new POSTagger());

    // Log result of load to file.
    if(load)
    {
        int success = loadTrainData();
        if(success != SUCCESS)
            train();
    }
    else
        train();
}

int SyntacticParser::transition(Transition move, int index, vector<int>& stack, Parse& parse)
{
    switch(move)
    {
        case SHIFT:
            stack.push_back(index);
            return index + 1;
        case LEFT:
            parse.addDependency(index, stack[stack.size()-1]);
            stack.pop_back();
            return index;
        case RIGHT:
            parse.addDependency(stack[stack.size()-2], stack[stack.size()-1]);
            stack.pop_back();
            return index;
    }
}


vector<Transition> SyntacticParser::validMoves(int index, int bufferSize, int stackSize)
{
    vector<Transition> valMoves;

    if(stackSize >= 2)
        valMoves.push_back(RIGHT);
    if(stackSize > 0)
        valMoves.push_back(LEFT);
    if(index < bufferSize)
        valMoves.push_back(SHIFT);

    return valMoves;
}

tuple<vector<string>, Parse> SyntacticParser::parse(vector<string>& words)
{
    Tokenizer tokenizer;
    vector<string> tags = tagger->tagWords(words);

    vector<string> context;
    for(auto& word : words)
    {
        context.push_back(tokenizer.normalize(word));
    }

    auto bufferSize = (int) context.size();
    auto index = 0;
    auto stack = vector<int>(); // start of with the root in the stack
    Parse dependencies(bufferSize);

    while (!stack.empty() || index < bufferSize)
    {
        auto features = getFeaturesForCurrentState(context, tags, index, bufferSize, stack, dependencies);
        map<Transition, double> scores = brain->score(features);
        vector<Transition> vMoves = validMoves(index, bufferSize, (int)stack.size());
        auto next_move = *(max_element(vMoves.begin(), vMoves.end(),
                                             [scores](const Transition p1, const Transition p2) {
                                                 return scores.find(p1)->second < scores.find(p2)->second; }));
        index = transition(next_move, index, stack, dependencies);
    }

    return tuple<vector<string>, Parse>(tags, dependencies);
}

std::string string_format(const std::string fmt, ...) {
    int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
    std::string str;
    va_list ap;
    while (1) {     // Maximum two passes on a POSIX system...
        str.resize(size);
        va_start(ap, fmt);
        int n = vsnprintf((char *)str.data(), size, fmt.c_str(), ap);
        va_end(ap);
        if (n > -1 && n < size) {  // Everything worked
            str.resize(n);
            return str;
        }
        if (n > -1)  // Needed size returned
            size = n + 1;   // For null char
        else
            size *= 2;      // Guess at a larger size (OS specific)
    }
    return str;
}

map<string, int> SyntacticParser::getFeaturesForCurrentState(vector<string>& words, vector<Tag> tags, int& n0, int& bufferSize, vector<int>& stack, Parse& dependencies)
{
    auto getStackContext = [](int& depth, vector<int>& stack, vector<string>& data)
                            {
                                if(depth >= 3)
                                    return tuple<string, string, string>(data[stack[stack.size()-1]], data[stack[stack.size()-2]], data[stack[stack.size()-3]]);
                                else if(depth >= 2)
                                    return tuple<string, string, string>(data[stack[stack.size()-1]], data[stack[stack.size()-2]], "");
                                else if(depth == 1)
                                    return tuple<string, string, string>(data[stack[stack.size()-1]], "", "");
                                else
                                    return tuple<string, string, string>("", "", "");
                            };

    auto getBufferContext = [](int& index, int& bufferSize, vector<string>& data)
                            {
                                if(index + 1 >= bufferSize)
                                    return tuple<string, string, string>(data[data.size()-1], "", "");
                                else if(index + 2 >= bufferSize)
                                    return tuple<string, string, string>(data[index], data[index+1], "");
                                else
                                    return tuple<string, string, string>(data[index], data[index+1], data[index+2]);
                            };

    auto getParseContex = [](int wordIndex, vector<vector<int>>& deps, vector<string>& data)
                          {
                              if(wordIndex == -1 || wordIndex >= data.size())
                                  return tuple<string, string, string>(to_string(0), "", "");

                              auto deps2 = deps[wordIndex];
                              auto valency = (int) deps2.size();

                              if(valency == 0)
                                  return tuple<string, string, string>(to_string(0), "", "");
                              else if(valency == 1)
                                  return tuple<string, string, string>(to_string(1), data[deps2[deps2.size()-1]], "");
                              else
                                  return tuple<string, string, string>(to_string(valency), data[deps2[deps2.size()-1]], data[deps2[deps2.size()-2]]);
                          };


    map<string, int> features;

    /*
     *  Feature extraction for parsing:
     *  - SO-2: Top three words on the stack
     *  - N0-2: First three words in the buffer
     *  - n0b1, n0b2: Two leftmost children of the first word of the buffer
     *  - s0b1, s0b2: Two leftmost children of the top word of the stack
     *  - s0f1, s0f2: Two rightmost children of the top word of the stack
     */

    auto depth = (int)stack.size();
    auto s0 = depth > 0 ? stack[stack.size()-1] : -1;

    auto Ws0_2 = getStackContext(depth, stack, words);
    auto Ts0_2 = getStackContext(depth, stack, tags);

    auto Wn0_2 = getBufferContext(n0, bufferSize, words);
    auto Tn0_2 = getBufferContext(n0, bufferSize, tags);

    auto Vn0b_Wn0b1_2 = getParseContex(n0, dependencies.lefts, words);
    auto Vn0b_Tn0b1_2 = getParseContex(n0, dependencies.lefts, tags);

    auto Vn0f_Wn0f1_2 = getParseContex(n0, dependencies.rights, words);
    auto Vn0f_Tn0f1_2 = getParseContex(n0, dependencies.rights, tags);

    auto Vs0b_Ws0b1_2 = getParseContex(s0, dependencies.lefts, words);
    auto Vs0b_Ts0b1_2 = getParseContex(s0, dependencies.lefts, tags);

    auto Vs0f_Ws0f1_2 = getParseContex(s0, dependencies.rights, words);
    auto Vs0f_Ts0f1_2 = getParseContex(s0, dependencies.rights, tags);

    auto wordContextVect = vector<tuple<string, string, string>>({Ws0_2, Wn0_2, Vn0b_Wn0b1_2, Vn0f_Wn0f1_2, Vs0b_Ws0b1_2, Vs0f_Ws0f1_2});
    auto  tagContextVect = vector<tuple<string, string, string>>({Ts0_2, Tn0_2, Vn0b_Tn0b1_2, Vn0f_Tn0f1_2, Vs0b_Ts0b1_2, Vs0f_Ts0f1_2});

    auto distanceS0N0 = s0 != 0 ? min((n0 - s0), 5) : 0;


    features["bias"] = 1;


    // Add word and tag unigrams
    for(int i = 0; i < wordContextVect.size(); i++)
    {
        if(i <= 1)
        {
            if(get<0>(wordContextVect[i]).compare("") != 0)
                features[string_format("w=%s", get<0>(wordContextVect[i]).c_str())];
            if(get<1>(wordContextVect[i]).compare("") != 0)
                features[string_format("w=%s", get<1>(wordContextVect[i]).c_str())];
            if(get<2>(wordContextVect[i]).compare("") != 0)
                features[string_format("w=%s", get<2>(wordContextVect[i]).c_str())];
        }
        else
        {
            if(get<1>(wordContextVect[i]).compare("") != 0)
                features[string_format("w=%s", get<1>(wordContextVect[i]).c_str())];
            if(get<2>(wordContextVect[i]).compare("") != 0)
                features[string_format("w=%s", get<2>(wordContextVect[i]).c_str())];
        }
    }

    for(int i = 0; i < tagContextVect.size(); i++)
    {
        if(i <= 1)
        {
            if(get<0>(tagContextVect[i]).compare("") != 0)
                features[string_format("t=%s", get<0>(tagContextVect[i]).c_str())];
            if(get<1>(tagContextVect[i]).compare("") != 0)
                features[string_format("t=%s", get<1>(tagContextVect[i]).c_str())];
            if(get<2>(tagContextVect[i]).compare("") != 0)
                features[string_format("t=%s", get<2>(tagContextVect[i]).c_str())];
        }
        else
        {
            if(get<1>(tagContextVect[i]).compare("") != 0)
                features[string_format("t=%s", get<1>(tagContextVect[i]).c_str())]++;
            if(get<2>(tagContextVect[i]).compare("") != 0)
                features[string_format("t=%s", get<2>(tagContextVect[i]).c_str())];
        }
    }

    // Add word/tag pairs
    for(int i = 0; i < 2; i++)
    {
        auto wTuple = wordContextVect[i];
        auto tTuple = tagContextVect[i];

        if(get<0>(wTuple).compare("") != 0 || get<0>(tTuple).compare("") != 0)
            features[string_format("%d w=%s, t=%s", i, get<0>(wTuple).c_str(), get<0>(tTuple).c_str())]++;
        if(get<1>(wTuple).compare("") != 0 || get<1>(tTuple).compare("") != 0)
            features[string_format("%d w=%s, t=%s", i, get<1>(wTuple).c_str(), get<1>(tTuple).c_str())]++;
        if(get<2>(wTuple).compare("") != 0 || get<2>(tTuple).compare("") != 0)
            features[string_format("%d w=%s, t=%s", i, get<2>(wTuple).c_str(), get<2>(tTuple).c_str())]++;
    }

    // Add bigrams
    features[string_format("s0w=%s, n0w=%s", get<0>(Ws0_2).c_str(), get<0>(Wn0_2).c_str())]++;
    features[string_format("wn0tn0-ws0 %s/%s %s", get<0>(Wn0_2).c_str(), get<0>(Tn0_2).c_str(), get<0>(Ws0_2).c_str())]++;
    features[string_format("wn0tn0-ts0 %s/%s %s", get<0>(Wn0_2).c_str(), get<0>(Tn0_2).c_str(), get<0>(Ts0_2).c_str())]++;
    features[string_format("ws0ts0-wn0 %s/%s %s", get<0>(Ws0_2).c_str(), get<0>(Ts0_2).c_str(), get<0>(Wn0_2).c_str())]++;
    features[string_format("ws0-ts0 tn0 %s/%s %s",get<0>(Ws0_2).c_str(), get<0>(Ts0_2).c_str(), get<0>(Tn0_2).c_str())]++;
    features[string_format("wt-wt %s/%s %s/%s", get<0>(Ws0_2).c_str(), get<0>(Ts0_2).c_str(), get<0>(Wn0_2).c_str(), get<0>(Tn0_2).c_str())]++;
    features[string_format("tt s0=%s n0=%s", get<0>(Ts0_2).c_str(), get<0>(Tn0_2).c_str())]++;
    features[string_format("tt n0=%s n1=%s", get<0>(Tn0_2).c_str(), get<1>(Tn0_2).c_str())]++;

    //  Add trigrams
    vector<tuple<string, string, string>> trigrams;
    trigrams.push_back(tuple<string, string, string>(get<0>(Tn0_2), get<1>(Tn0_2), get<2>(Tn0_2)));
    trigrams.push_back(tuple<string, string, string>(get<0>(Ts0_2), get<0>(Tn0_2), get<1>(Tn0_2)));
    trigrams.push_back(tuple<string, string, string>(get<0>(Ts0_2), get<1>(Ts0_2), get<0>(Tn0_2)));
    trigrams.push_back(tuple<string, string, string>(get<0>(Ts0_2), get<1>(Vs0f_Ts0f1_2), get<0>(Tn0_2)));
    trigrams.push_back(tuple<string, string, string>(get<0>(Ts0_2), get<0>(Tn0_2), get<1>(Vs0f_Ts0f1_2)));
    trigrams.push_back(tuple<string, string, string>(get<0>(Ts0_2), get<0>(Tn0_2), get<1>(Vn0b_Tn0b1_2)));
    trigrams.push_back(tuple<string, string, string>(get<0>(Ts0_2), get<1>(Vs0b_Ts0b1_2), get<2>(Vs0b_Ts0b1_2)));
    trigrams.push_back(tuple<string, string, string>(get<0>(Ts0_2), get<1>(Vs0f_Ts0f1_2), get<2>(Vs0f_Ts0f1_2)));
    trigrams.push_back(tuple<string, string, string>(get<0>(Tn0_2), get<1>(Vn0b_Tn0b1_2), get<2>(Vn0b_Tn0b1_2)));
    trigrams.push_back(tuple<string, string, string>(get<0>(Ts0_2), get<1>(Ts0_2), get<1>(Ts0_2)));

    auto i = 0;
    for(auto& tup : trigrams)
        if(get<0>(tup).compare("") != 0 || get<1>(tup).compare("") != 0 || get<2>(tup).compare("") != 0)
        {
            features[string_format("ttt-%d %s %s %s", i, get<0>(tup).c_str(), get<1>(tup).c_str(), get<2>(tup).c_str())]++;
            i++;
        }

    //  Add some valency and dist features
    vector<tuple<string, string>> vect;
    vect.push_back(tuple<string, string>(get<0>(Ws0_2), get<0>(Vs0f_Ws0f1_2)));
    vect.push_back(tuple<string, string>(get<0>(Ws0_2), get<0>(Vs0b_Ws0b1_2)));
    vect.push_back(tuple<string, string>(get<0>(Wn0_2), get<0>(Vn0b_Wn0b1_2)));

    vect.push_back(tuple<string, string>(get<0>(Ts0_2), get<0>(Vs0f_Ts0f1_2)));
    vect.push_back(tuple<string, string>(get<0>(Ts0_2), get<0>(Vs0b_Ts0b1_2)));
    vect.push_back(tuple<string, string>(get<0>(Tn0_2), get<0>(Vn0b_Tn0b1_2)));

    vect.push_back(tuple<string, string>(get<0>(Ws0_2), to_string(distanceS0N0)));
    vect.push_back(tuple<string, string>(get<0>(Wn0_2), to_string(distanceS0N0)));
    vect.push_back(tuple<string, string>(get<0>(Ts0_2), to_string(distanceS0N0)));
    vect.push_back(tuple<string, string>(get<0>(Tn0_2), to_string(distanceS0N0)));
    vect.push_back(tuple<string, string>(string("t") + get<0>(Tn0_2) + get<0>(Ts0_2), to_string(distanceS0N0)));
    vect.push_back(tuple<string, string>(string("w") + get<0>(Wn0_2) + get<0>(Ws0_2), to_string(distanceS0N0)));

    for (int i = 0; i < vect.size(); i++)
    {
        auto tup = vect[i];
        if(get<0>(tup).compare("") != 0 || get<1>(tup).compare("") != 0)
            features[string_format("val/d-%d %s %d", i, get<0>(tup).c_str(), get<1>(tup).c_str())]++;
    }

    return features;
}

string SyntacticParser::readCorpus(string& location)
{
    auto files = vector<string>();
    auto start = 1;
    auto end = 199;

    location = (location.compare("") == 0) ? "Corpus/" : location;
    stringstream stream;

    for (int i = start; i < end; i++)
    {
        string fileNumber = "";
        if(i < 10)
            fileNumber = string("00") + to_string(i);
        else if(i < 100)
            fileNumber = string("0") + to_string(i);
        else
            fileNumber = to_string(i);

        string fileString = location + string("wsj_0") + fileNumber + string(".dp");
        ifstream in(fileString);
        if (in)
        {
            stream << in.rdbuf();
            stream << "\n";
            in.close();
        }
    }
    return (stream.str());
}

vector<Transition> SyntacticParser::getGoldMoves(int index, int bufferSize, vector<int>& stack, vector<int>& predictions, vector<int>& goldHeads)
{
    auto containsDependency = [](int& targetWordIdx, vector<int>& otherWordsIdx, vector<int>& gold)
    {
        for(auto const& wordIdx : otherWordsIdx)
            if(gold[wordIdx] == targetWordIdx || gold[targetWordIdx] == wordIdx)
                return true;
        return false;
    };

    auto vMoves = validMoves(index, bufferSize, (int)stack.size());
    auto tmpVect = vector<int>({index});
    if(stack.empty() || (find(vMoves.begin(), vMoves.end(), SHIFT) != vMoves.end() && goldHeads[index] == stack[stack.size()-1]))
        return vector<Transition>({SHIFT});
    if(goldHeads[stack[stack.size()-1]] == index)
        return vector<Transition>({LEFT});

    auto vMovesSet = set<Transition>(vMoves.begin(), vMoves.end());
    auto allMoves = set<Transition>({SHIFT, LEFT, RIGHT});
    auto costlyMoves = set<Transition>();
    for(auto& move : allMoves)
        if(vMovesSet.find(move) == vMovesSet.end())
            costlyMoves.insert(move);

    //  If the word behind the first element in the stack is it's gold head, then LEFT move is incorrect
    if(stack.size() >= 2 && goldHeads[stack[stack.size()-1]] == stack[stack.size()-2])
        costlyMoves.insert(LEFT);

    //  If dependencies exist between the first element in the buffer and the first element in the stack,
    //  SHIFTing will lose them
    if(costlyMoves.find(SHIFT) == costlyMoves.end() && containsDependency(index, stack, goldHeads))
        costlyMoves.insert(SHIFT);

    //  If there are any dependencies between s0 and the buffer, popping s0 from the stack will loose them
    vector<int> range;
    for(int i = index+1; i < bufferSize; i++)
        range.push_back(i);

    if(containsDependency(*(stack.rbegin()), range, goldHeads))
    {
        costlyMoves.insert(LEFT);
        costlyMoves.insert(RIGHT);
    }

    return [](set<Transition>& allMoves, set<Transition>& costlyMoves)
    {
        vector<Transition> goldMoves;

        for(auto m : allMoves)
            if(costlyMoves.find(m) == costlyMoves.end())
                goldMoves.push_back(m);

        return goldMoves;
    }(allMoves, costlyMoves);
}



int SyntacticParser::trainOne(int iteration, vector<string>& words, vector<int>& goldHeads)
{
    int bufferSize = (int) words.size();
    int index = 1;
    vector<int> stack = vector<int>({0});
    Parse dependencies(bufferSize);

    auto getNumCorrectPredictions = [words](Parse& prediction, vector<int>& goldHeads)
    {
        int numCorrect = 0;
        for(int i = 0; i < prediction.heads.size(); i++)
        {
            if(prediction.heads[i] >= prediction.numWords)
                numCorrect += (-1 == goldHeads[i]) ? 1 : 0;
            else if(prediction.heads[i] == goldHeads[i])
                numCorrect++;
        }
        return numCorrect;
    };

    auto trueNextParse = [this, getNumCorrectPredictions](int index, int bufferSize, vector<int>& stack, Parse& dep, vector<int>& gold)
    {
        map<Transition, int>scores;

        for(auto& move : this->validMoves(index, bufferSize, (int)stack.size()))
        {
            Parse cp = Parse(dep);
            vector<int> cpStack = stack;
            transition(move, index, cpStack, cp);
            scores[move] = getNumCorrectPredictions(cp, gold);
        }
        auto oldAcc = getNumCorrectPredictions(dep, gold);
        if(oldAcc == scores[SHIFT] && oldAcc == scores[LEFT] && oldAcc == scores[RIGHT])
            return SHIFT;

        Transition next = (max_element(scores.begin(), scores.end(),
                                  [scores](const pair<Transition, int> p1, const pair<Transition, int> p2) {
                                      return p1.second < p2.second; }))->first;
        return next;
    };

    auto tags = tagger->tagWords(words);
    while(!stack.empty() || index < bufferSize)
    {
        auto features = getFeaturesForCurrentState(words, tags, index, bufferSize, stack, dependencies);
        auto scores = brain->score(features);
        auto vMoves = validMoves(index, bufferSize, (int)stack.size());
        auto guess = scores.size() > 0 ? *(max_element(vMoves.begin(), vMoves.end(),
                                                       [scores](const Transition p1, const Transition p2) {
                                                           return scores.find(p1)->second < scores.find(p2)->second; })) : *(vMoves.begin());
        auto goldMoves = getGoldMoves(index, bufferSize, stack, dependencies.heads, goldHeads);
        auto best = goldMoves.size() > 0 ? *(max_element(goldMoves.begin(), goldMoves.end(),
                                                      [scores](const Transition p1, const Transition p2) {
                                                          return scores.find(p1)->second < scores.find(p2)->second; })) : trueNextParse(index, bufferSize, stack, dependencies, goldHeads);
        brain->updateFeatureWeights(best, guess, features);
        index = explore(guess, best, iteration, index, stack, dependencies);
    }

    return getNumCorrectPredictions(dependencies, goldHeads);
}

int SyntacticParser::explore(Transition guess, Transition truth, int iteration, int index, vector<int>& stack, Parse& dep)
{
    if (iteration > 5 && ((double) rand() / (RAND_MAX)) < 0.85)
        return transition(guess, index, stack, dep);
    else
        return transition(truth, index, stack, dep);
}

////////////////////////////////////////////////////////////////////
//  Train-Test Code is has redundancies.
//  TODO: Clean-up test code and make it more compact
////////////////////////////////////////////////////////////////////
int SyntacticParser::trainOneOut()
{
    auto getNumCorrectPredictions = [](Parse& prediction, vector<int>& goldHeads)
    {
        int numCorrect = 0;
        for(int i = 0; i < prediction.heads.size(); i++)
        {
            if(prediction.heads[i] >= prediction.numWords)
                numCorrect += (-1 == goldHeads[i]) ? 1 : 0;
            else if(prediction.heads[i] == goldHeads[i])
                numCorrect++;
        }
        return numCorrect;
    };

    string corpusLocation = "./DPCorpus/";
    auto corpus = readCorpus(corpusLocation);

    if(corpus.compare("") == 0)
    {
        return FAILURE;
    }


    Tokenizer tokenizer;

    int loopCount = 0;
    float total = 0;
    while(true)
    {
        auto sentences = tokenizer.tokenizeSentence(corpus);

        auto lIdx = 0;
        vector<string> trueSentVect;
        while(lIdx < sentences.size())
        {
            stringstream ss;
            while(sentences[lIdx].compare("") != 0)
            {
                ss << sentences[lIdx] << " ";
                lIdx++;
            }
            lIdx++;
            trueSentVect.push_back(ss.str());
            ss.str("");
        }

        sentences = trueSentVect;

        //  break out after size() iterations
        if(loopCount >= sentences.size()-1)
            break;

        auto tenPercent = 1;
        vector<string> testData;

        for(int i = 0; i < tenPercent; i++)
        {
            testData.push_back(sentences[sentences.size()-1]);
            sentences.pop_back();
        }

        for(int iter = 0; iter < 10; iter++)
        {
            int numCorrect = 0;
            int count = 0;
            random_shuffle(sentences.begin(), sentences.end());

            for (auto sentence : sentences)
            {
                vector<string> context;
                vector<int> goldHeads;
                auto words = tokenizer.tokenizeWord(sentence);
                for (auto rawWord : words)
                {
                    auto tup = tokenizer.extractWordGoldHead(rawWord);
                    context.push_back(get<0>(tup));
                    goldHeads.push_back(get<1>(tup)-1);
                }

                numCorrect += trainOne(iter, context, goldHeads);
                count += goldHeads.size();
            }
            //cout << "Accuracy: " << iter+1 << " " << (float(numCorrect)/float(count) * 100) << endl;
        }
        brain->averageWeights();

        int numCorrect = 0;
        int count = 0;
        for(string sentence : testData)
        {
            vector<string> context;
            vector<int> goldHeads;
            auto words = tokenizer.tokenizeWord(sentence);
            for (auto rawWord : words)
            {
                auto tup = tokenizer.extractWordGoldHead(rawWord);
                context.push_back(get<0>(tup));
                goldHeads.push_back(get<1>(tup)-1);
            }

            auto result = parse(context);
            numCorrect += getNumCorrectPredictions(get<1>(result), goldHeads);
            count += goldHeads.size();
        }

        total += (float)numCorrect/(float)count * 100.0;
        loopCount++;
    }

    cout << "Accuracy Leave-One-Out: " << total/10.0 << endl;
    return SUCCESS;
}

int SyntacticParser::trainTenFold()
{

    auto getNumCorrectPredictions = [](Parse& prediction, vector<int>& goldHeads)
    {
        int numCorrect = 0;
        for(int i = 0; i < prediction.heads.size(); i++)
        {
            if(prediction.heads[i] >= prediction.numWords)
                numCorrect += (-1 == goldHeads[i]) ? 1 : 0;
            else if(prediction.heads[i] == goldHeads[i])
                numCorrect++;
        }
        return numCorrect;
    };

    string corpusLocation = "./DPCorpus/";
    auto corpus = readCorpus(corpusLocation);

    if(corpus.compare("") == 0)
    {
        return FAILURE;
    }


    Tokenizer tokenizer;

    float total = 0;
    for(int t = 0; t < 10; t++)
    {
        auto sentences = tokenizer.tokenizeSentence(corpus);

        auto lIdx = 0;
        vector<string> trueSentVect;
        while(lIdx < sentences.size())
        {
            stringstream ss;
            while(sentences[lIdx].compare("") != 0)
            {
                ss << sentences[lIdx] << " ";
                lIdx++;
            }
            lIdx++;
            trueSentVect.push_back(ss.str());
            ss.str("");
        }

        sentences = trueSentVect;

        auto tenPercent = (int) sentences.size() * 0.1;
        vector<string> testData;

        for(int i = 0; i < tenPercent; i++)
        {
            testData.push_back(sentences[sentences.size()-1]);
            sentences.pop_back();
        }

        for(int iter = 0; iter < 10; iter++)
        {
            int numCorrect = 0;
            int count = 0;
            random_shuffle(sentences.begin(), sentences.end());

            for (auto sentence : sentences)
            {
                vector<string> context;
                vector<int> goldHeads;
                auto words = tokenizer.tokenizeWord(sentence);
                for (auto rawWord : words)
                {
                    auto tup = tokenizer.extractWordGoldHead(rawWord);
                    context.push_back(get<0>(tup));
                    goldHeads.push_back(get<1>(tup)-1);
                }

                numCorrect += trainOne(iter, context, goldHeads);
                count += goldHeads.size();
            }
            //cout << "Accuracy: " << iter+1 << " " << (float(numCorrect)/float(count) * 100) << endl;
        }
        brain->averageWeights();

        int numCorrect = 0;
        int count = 0;
        for(string sentence : testData)
        {
            vector<string> context;
            vector<int> goldHeads;
            auto words = tokenizer.tokenizeWord(sentence);
            for (auto rawWord : words)
            {
                auto tup = tokenizer.extractWordGoldHead(rawWord);
                context.push_back(get<0>(tup));
                goldHeads.push_back(get<1>(tup)-1);
            }

            auto result = parse(context);
            numCorrect += getNumCorrectPredictions(get<1>(result), goldHeads);
            count += goldHeads.size();
        }

        total += (float)numCorrect/(float)count * 100.0;
    }

    cout << "Accuracy Ten-Fold: " << total/10.0 << endl;
    return SUCCESS;
}
////////////////////////////////////////////////////////////////////////////

int SyntacticParser::train()
{
    string corpusLocation = "./DPCorpus/";
    auto corpus = readCorpus(corpusLocation);

    if(corpus.compare("") == 0)
    {
        return FAILURE;
    }


    Tokenizer tokenizer;
    auto sentences = tokenizer.tokenizeSentence(corpus);

    auto lIdx = 0;
    vector<string> trueSentVect;
    while(lIdx < sentences.size())
    {
        stringstream ss;
        while(sentences[lIdx].compare("") != 0)
        {
            ss << sentences[lIdx] << " ";
            lIdx++;
        }
        lIdx++;
        trueSentVect.push_back(ss.str());
        ss.str("");
    }

    sentences = trueSentVect;

    for(int iter = 0; iter < 10; iter++)
    {
        int numCorrect = 0;
        int count = 0;
        random_shuffle(sentences.begin(), sentences.end());

        for (auto sentence : sentences)
        {
            vector<string> context;
            vector<int> goldHeads;
            auto words = tokenizer.tokenizeWord(sentence);
            for (auto rawWord : words)
            {
                auto tup = tokenizer.extractWordGoldHead(rawWord);
                context.push_back(get<0>(tup));
                goldHeads.push_back(get<1>(tup)-1);
            }

            numCorrect += trainOne(iter, context, goldHeads);
            count += goldHeads.size();
        }
        cout << "Accuracy: " << iter+1 << " " << (float(numCorrect)/float(count) * 100) << endl;
    }
    brain->averageWeights();

    saveTrainData();

    return SUCCESS;
}

int SyntacticParser::loadTrainData()
{
    // Things to load
    //  perceptron weights

    string prefixLocationPath = "./Train Data/DP/";
    ifstream weightsReader(prefixLocationPath + string("/Weights.pos"));

    if(!weightsReader.is_open())
        return FAILURE;


    //  Retrieve weights
    weightsReader >> *brain;
    weightsReader.close();

    return SUCCESS;
}

int SyntacticParser::saveTrainData()
{
    // Things to save
    //  perceptron weights


    string prefixLocationPath = "./Train Data/DP/";
    ofstream weightsWriter(prefixLocationPath + string("/Weights.pos"));
    if(!weightsWriter.is_open())
        return FAILURE;

    //  Save weights
    weightsWriter << *brain;
    weightsWriter.close();

    return SUCCESS;
}
