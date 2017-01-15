#include "POSTagger.h"
#include "Tokenizer.h"
#include "errorCodes.h"
#include <iostream>
#include <numeric>
#include <regex>

POSTagger::POSTagger(bool load)
{
    classes = make_shared<set<Tag>>();
    knownTags = unique_ptr<unordered_map<string, Tag>>(new unordered_map<string, Tag>());
    brain = unique_ptr<POSAveragePerceptron>(new POSAveragePerceptron());

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

int POSTagger::getCorpusCategories(vector<Category>& categoryList)
{
    /*
    * DEFAULT: Brown corpus
    * Corupus Syntax: word/POS
    * ca01 - ca44 news
    * cb01 - cb27 editorial
    * cc01 - cc17 reviews
    * cd01 - cd17 religion
    * ce01 - ce36 hobbies
    * cf01 - cf48 lores
    * cg01 - cg75 belles_lettres
    * ch01 - ch30 government
    * cj01 - cj80 learned
    * ck01 - ck29 fiction
    * cl01 - cl24 mystery
    * cm01 - cm06 science_fiction
    * cn01 - cn29 adventure
    * cp01 - cp29 romance
    * cr01 - cr09 humor
    */
    categoryList.push_back(Category("news", "ca", 1, 44));
    categoryList.push_back(Category("editorial", "cb", 1, 27));
    categoryList.push_back(Category("reviews", "cc", 1, 17));
    categoryList.push_back(Category("religion", "cd", 1, 17));
    categoryList.push_back(Category("hobbies", "ce", 1, 36));
    categoryList.push_back(Category("lores", "cf", 1, 48));
    categoryList.push_back(Category("belles_lettres", "cg", 1, 75));
    categoryList.push_back(Category("government", "ch", 1, 30));
    categoryList.push_back(Category("learned", "cj", 1, 80));
    categoryList.push_back(Category("fiction", "ck", 1, 29));
    categoryList.push_back(Category("mystery", "cl", 1, 24));
    categoryList.push_back(Category("science_fiction", "cm", 1, 6));
    categoryList.push_back(Category("adventure", "cn", 1, 29));
    categoryList.push_back(Category("romance", "cp", 1, 29));
    categoryList.push_back(Category("humor", "cr", 1, 9));

    return SUCCESS;
}


string POSTagger::readCorpus(string& location)
{
    auto categories = vector<Category>();

    if(getCorpusCategories(categories))
    {
        location = (location.compare("") == 0) ? "Corpus/" : location;
        stringstream stream;

        for (auto category : categories)
        {
            for(auto j = 1; j <= category.end; j++)
            {
                auto fileNumber =  (j < 10) ? "0" + to_string(j) : to_string(j);
                string fileString = location + category.prefixTag + fileNumber;
                ifstream in(fileString);
                if (in)
                {
                    stream << in.rdbuf();
                    in.close();
                }
            }
        }
        return (stream.str());
    }
    return "";
}


map<string, int> POSTagger::getFeaturesForWord(int i, string& word, vector<string>& context, string& prevTag, string& prev2Tag)
{
    /*
    *  Using features from Hannibol (POS Blog)
    *  -> i word suffix (3 chars)
    *  -> i word prefix (1 char)
    *  -> i-1 tag
    *  -> i-2 tag
    *  -> i-1 tag + i-2 tag
    *  -> i word
    *  -> i-1 tag + i word
    *  -> i-1 word
    *  -> i-1 word suffix (3 chars)
    *  -> i-2 word
    *  -> i+1 word
    *  -> i+1 word suffix
    *  -> i+2 word
    */
    map<string, int> features;

    auto addFeature = [&features] (string name, string values) {
     features[name + string(",") + values] += 1;
    };

    //Account For padding
    i += 2;

    addFeature(string("i word suffix"), word.length() >= 3 ? word.substr(word.length()-3) : word);
    addFeature(string("i word prefix"), string(1, word[0]));
    addFeature(string("i-1 tag"), prevTag);
    addFeature(string("i-2 tag"), prev2Tag);
    addFeature(string("i-1 tag + i-2 tag"), prevTag + prev2Tag);
    addFeature(string("i word"), word);
    addFeature(string("i-1 tag + i word"), prevTag + word);
    addFeature(string("i-1 word"), context[i-1]);
    addFeature(string("i-1 word suffix"), context[i-1].length() >= 3 ? context[i-1].substr(context[i-1].length()-3) : context[i-1]);
    addFeature(string("i-2 word"), context[i-2]);
    addFeature(string("i+1 word"), context[i+1]);
    addFeature(string("i+1 word suffix"), context[i+1].length() >= 3 ?context[i+1].substr(context[i+1].length()-3) : context[i+1]);
    addFeature(string("i+2 word"), context[i+2]);

    return features;
}

void POSTagger::populateKnownTags(vector<string>& sentences)
{
    Tokenizer tokenizer;
    map<string, map<Tag,int>> count;

    for (auto sentence : sentences)
    {
        auto words = tokenizer.tokenizeWord(sentence);
        words = tokenizer.wordsBrownToPenn(words);

        for (auto word_tag : words)
        {
            auto tmp = tokenizer.extractWordTag(word_tag);
            auto word = get<0>(tmp);
            auto tag = get<1>(tmp);

            count[word][tag] += 1;
            classes->insert(tag);
        }
    }

    //  Only keep track of unambiguous & frequently occuring word tags
    double frequencyThreshold = 20;
    double ambiguityThreshold = 0.97;

    for (auto const& word_TagsFreqs : count)
    {
        string word = word_TagsFreqs.first;
        auto maxE = max_element(word_TagsFreqs.second.begin(), word_TagsFreqs.second.end(),
                                    [](const pair<Tag, int>& p1, const pair<Tag, int>& p2) {
                                        return p1.second < p2.second; });
        Tag mstFrqntTagForWord = maxE->first;
        int maxFreq = maxE->second;

        int totalFreqForWord = accumulate(word_TagsFreqs.second.begin(), word_TagsFreqs.second.end(),
                                           0, [] (int value, const map<string, int>::value_type& p)
                                                  { return value + p.second; });
        if (maxFreq >= frequencyThreshold && double(maxFreq)/totalFreqForWord >= ambiguityThreshold)
        {
            (*knownTags)[word] = mstFrqntTagForWord;
        }
    }
    brain->setClasses(*classes);
}


// assume each sentence in corpus has the "\n" at the end
int POSTagger::train()
{
    string corpusLocation = "./POSCorpus/";
    auto corpus = readCorpus(corpusLocation);

    if(corpus.compare("") == 0)
    {
        return FAILURE;
    }


    Tokenizer tokenizer;
    auto sentences = tokenizer.tokenizeSentence(corpus);
    string prev1 = "-START-";
    string prev2 = "-START1-";
    populateKnownTags(sentences);


    for(int iter = 0; iter < 5; iter++)
    {
        int numCorrect = 0;
        int count = 0;
        random_shuffle(sentences.begin(), sentences.end());

        for (auto sentence : sentences)
        {
            vector<string> context;
            //add padding to context for getFeatures
            context.push_back("-START-");
            context.push_back("-START1");
            auto words = tokenizer.tokenizeWord(sentence);
            words = tokenizer.wordsBrownToPenn(words);
            for (auto word_tag : words)
            {
                context.push_back(get<0>(tokenizer.extractWordTag(word_tag)));
            }
            context.push_back("-END-");
            context.push_back("-END1");

            int i = 0;

            for (auto word_tag : words)
            {
                word_tag = tokenizer.trim(word_tag);
                if(word_tag.compare("") == 0)
                    continue;

                auto tmp = tokenizer.extractWordTag(word_tag);
                auto word = get<0>(tmp);
                Tag trueTag = get<1>(tmp);

                Tag guess = knownTags->find(word) == knownTags->end() ? "" : (*knownTags)[word];
                if (guess.compare("") == 0)
                {
                    auto featuresForWord = getFeaturesForWord(i, word, context, prev1, prev2);
                    guess = brain->predictTag(featuresForWord);
                    brain->updateFeatureWeights(trueTag, guess, featuresForWord);
                }

                prev2 = prev1;
                prev1 = guess;
                numCorrect += (guess.compare(trueTag) == 0) ? 1 : 0;
                count++;
                i++;
            }
        }
        cout << "Accuracy: " << iter+1 << " " << (float(numCorrect)/float(count) * 100) << endl;
    }
    brain->averageWeights();

    saveTrainData();

    return SUCCESS;
}

int POSTagger::loadTrainData()
{
    // Things to load
    //  perceptron weights
    //  known word - tags
    //  all possible tags

    string prefixLocationPath = "./Train Data/POS/";
    ifstream allTagsReader(prefixLocationPath + string("/AllTags.pos"));
    ifstream knownWordTagsReader(prefixLocationPath + string("/KnownWords.pos"));
    ifstream weightsReader(prefixLocationPath + string("/Weights.pos"));

    if(!allTagsReader.is_open() || !knownWordTagsReader.is_open() || !weightsReader.is_open())
        return FAILURE;

    //  Retrieve all possible tags
    Tag tag;
    while(allTagsReader >> tag)
    {
        classes->insert(tag);
    }
    allTagsReader.close();
    brain->setClasses(*classes);

    //  Retrieve known word tags
    regex e("(.+) : (.+)");
    smatch sm;
    char wordTag[256];
    while(knownWordTagsReader.getline(wordTag, 256) && regex_match(string(wordTag), sm, e))
    {
        (*knownTags)[sm[1].str()] = sm[2].str();
    }
    knownWordTagsReader.close();


    //  Retrieve weights
    weightsReader >> *brain;
    weightsReader.close();

    return SUCCESS;
}

int POSTagger::saveTrainData()
{
    // Things to save
    //  perceptron weights
    //  known word - tags
    //  all possible tags


    string prefixLocationPath = "./Train Data/POS/";

    ofstream allTagsWriter(prefixLocationPath + string("/AllTags.pos"));
    ofstream knownWordTagsWriter(prefixLocationPath + string("/KnownWords.pos"));
    ofstream weightsWriter(prefixLocationPath + string("/Weights.pos"));
    if(!allTagsWriter.is_open() || !knownWordTagsWriter.is_open() || !weightsWriter.is_open())
        return FAILURE;

    //  Save all possible tags
    for(auto const& tag : *classes)
    {
        allTagsWriter << tag << endl;
    }

    allTagsWriter.close();


    //  Save known word tags
    for(auto const& word_tag : *knownTags)
    {
        knownWordTagsWriter << word_tag.first << " : " << word_tag.second << endl;
    }
    knownWordTagsWriter.close();


    //  Save weights
    weightsWriter << *brain;
    weightsWriter.close();

    return SUCCESS;
}

vector<Tag> POSTagger::tagWords(vector<string> words)
{
    vector<string> context;
    Tokenizer tokenizer;
    vector<Tag> tags;

    //add padding to context for getFeatures
    string prev1 = "-START-";
    string prev2 = "-START1-";
    context.push_back("-START-");
    context.push_back("-START1");

    for (auto word : words)
    {
        context.push_back(tokenizer.normalize(word));
    }
    context.push_back("-END-");
    context.push_back("-END1");

    int i = 0;
    for (auto& word : words)
    {
        string cpWord = word;
        cpWord = tokenizer.normalize(cpWord);

        Tag guess = knownTags->find(cpWord) == knownTags->end() ? "" : (*knownTags)[cpWord];
        if (guess.compare("") == 0)
        {
            auto featuresForWord = getFeaturesForWord(i, cpWord, context, prev1, prev2);
            guess = brain->predictTag(featuresForWord);
        }

        prev2 = prev1;
        prev1 = guess;
        tags.push_back(guess);
        i++;
    }

    return tags;
}


vector<Tag> POSTagger::tagString(string& corpus)
{
    Tokenizer tokenizer;

    auto words = tokenizer.tokenizeWord(corpus);
    return tagWords(words);
}
