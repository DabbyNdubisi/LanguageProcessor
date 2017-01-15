//
//  main.cpp
//  LanguageProcessor
//
//  Created by Dabby Ndubisi on 2016-03-21.
//  Copyright © 2016 Dabby Ndubisi. All rights reserved.
//

#include "POSTagger.h"
#include "Tokenizer.h"
#include "SyntaxParser.h"
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;

static int currIndex = 0;

int getChoice()
{
    int x;
    cout << "Enter choice: ";
    cin >> x;

    return x;
}

void drawTree(vector<string> words, vector<string> tags, Parse& parse)
{
    stringstream ss;

    ss << "digraph G { " << endl;
    ss << "ROOT [label=\"ROOT\"]" << endl;

    for(int i = 0; i < words.size(); i++)
    {
        ss << words[i] << "[label=\"" << words[i] << "\"]" << endl;
    }

    for(int i = 0; i < parse.heads.size(); i++)
    {
        if(parse.heads[i] >= words.size())
            ss << "ROOT -> " << words[i] << endl;
        else
            ss << words[parse.heads[i]] << " -> " << words[i] << endl;
    }

    ss << "}" << endl;

    string file = string("./Parses/output") + to_string(currIndex) + string(".dot");
    currIndex++;

    ofstream of(file.c_str());
    if(of.is_open())
    {
        of << ss.str();
    }

    cout << "Tree written to file with path: " << file << endl;
    cout << "In order to view tree, run command: dot -T png -O " << file << endl;
}

int main()
{
    Tokenizer tokenizer;
    cout << "Initializing POS tagger..." << endl;
    POSTagger tagger;
    cout << "Done Initializing" << endl;
    cout << "Initializing Syntax Parser..." << endl;
    SyntacticParser parser;
    cout << "Done Initializing" << endl;


    while(true)
    {
        cout << "Select option" << endl;
        cout << "(1) Tag sentence with POS tagger" << endl;
        cout << "(2) Parse Sentence" << endl;
        cout << "(0) Quit" << endl;

        int choice = getChoice();
        string sentence;

        if(choice == 0)
        {
            break;
        }
        else if(choice == 1)
        {
            while(true)
            {
                cout << "Please Enter Sentece To Tag (STOP to exit): ";
                while(true)
                {
                    getline(cin, sentence);
                    if(sentence.compare("") != 0)
                        break;
                }


                if(sentence.compare("STOP") == 0)
                    break;

                auto words = tokenizer.tokenizeWord(sentence);
                auto taggedSentence = tagger.tagWords(words);

                auto i = 0;
                for(auto tag : taggedSentence)
                {
                    cout << words[i] << " : " << tag << endl;
                    i++;
                }
                cout << endl;
            }
        }
        else if (choice == 2)
        {
            while(true)
            {
                cout << "Please Enter Sentece To Parse (STOP to exit): ";
                while(true)
                {
                    getline(cin, sentence);
                    if(sentence.compare("") != 0)
                        break;
                }

                if(sentence.compare("STOP") == 0)
                    break;

                auto words = tokenizer.tokenizeWord(sentence);
                auto parsedSentence = parser.parse(words);

                auto ansWords = get<0>(parsedSentence);
                auto parse = get<1>(parsedSentence);

                for(int i = 0; i < ansWords.size(); i++)
                {
                    cout << ansWords[i] << " : " << parse.heads[i] << endl;
                }
                cout << endl;

                drawTree(words, ansWords, parse);
            }
        }

    }
}
