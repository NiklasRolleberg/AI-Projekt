#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <limits>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include "wordmap.hpp"
#include <sstream>

class model
{
private:
    int N;// = 9; States
    int M;// = 9; Observations
    std::unordered_map<std::string, double> mapBigrams;
    std::unordered_map<std::string, double> mapTrigrams;
    std::unordered_set<std::string> trigrams;
    WordMap oneMap;
    double* weight;

public:
    /**Contructor - generate matrices*/
    model(int n, int m, WordMap inMap)
    {
        N=n;
        M=m;
        std::cerr << "constructor called size: " << N << "x" << M << std::endl;
        oneMap = inMap;
        weight = (double*)calloc(N,sizeof(double));
        for(int i=0;i<N;++i)
            weight[i] = 1.0;
    }

    std::string toString1(int one, int two)
    {
        std::stringstream ss;
        ss.str(""); // empty the stringstream
        ss.clear();
        ss << one;
        ss << " ";
        ss << two;
        std::string str2 = ss.str();
        return str2;
    }

    std::string toString2(int one, int two, int three)
    {
        std::stringstream ss;
        ss.str(""); // empty the stringstream
        ss.clear();
        ss << one;
        ss << " ";
        ss << two;
        ss << " ";
        ss << three;
        std::string str2 = ss.str();
        return str2;
    }

    /**uses the sequence "input" to train the hmm-model
    * input = the emission sequence
    */
    void learn(std::vector<int> in)
    {
        for (int i = in.size()-1; i > 1 ; i--)
        {
            //std::vector<int> temp1 = {in[i],in[i-1]}, temp2 = {in[i],in[i-1],in[i-2]};
            //std::string temp1 = ""; temp1.append(in[i]); temp1.append(" "); temp1.append(in[i-1]);
            //std::string temp1 = ""; temp1.append(in[i]); temp1.append(" "); temp1.append(in[i-1]);

            std::string temp1 = toString1(in[i],in[i-1]);
            std::string temp2 = toString2(in[i],in[i-1], in[i-2]);

            //std::string temp2 = ""+in[i]+" "+in[i-1]+" "+in[i-2];
            //int* tempBi =  temp1.data();
            mapBigrams[temp1]++; //If not previously existing (new bigram), automatically creates a node with value 0+1=1. Else, add 1 to value (count).
            //int* tempTri = temp2.data();
            mapTrigrams[temp2]++;
            trigrams.insert(temp2);
        }
    }

    /**Generate sequence*/
    std::vector<int> Generate(int lastWrd, int length)
    {
        //std::cout << "1Generation: " << lastWrd << std::endl;
        std::vector<int> out;

        //If "superlong" word LastWord (just in case)
        if (length - oneMap.syllables(oneMap.intToWord[lastWrd]) < 0)
        {
            out.push_back(lastWrd);
            return out;
        }

        //Consider
        int bestWrd1;
        double maxFreq = 0, tempVal1, tempVal2;
        tempVal2 = count(oneMap.sequence.begin(), oneMap.sequence.end(), lastWrd);
        for(int i = 0; i < mapBigrams.size(); i++)
        {
            //std::vector<int> temp1 = {lastWrd,i};
            std::string temp1 = toString1(lastWrd,i);
            //int* tempBi =  temp1.data();
            if (mapBigrams.find(temp1) != mapBigrams.end())
            {
                //std::cout << "ok\t" << temp1 << "\t" << mapBigrams[temp1] << std::endl;
                tempVal1 = (double)(1.+mapBigrams[temp1]);
            }
            else
                tempVal1 = 0;
            if ((tempVal1/tempVal2)*weight[i] > maxFreq)
            {
                bestWrd1 = i;
                maxFreq = (tempVal1/tempVal2)*weight[i];
            }
        }

        weight[lastWrd] *=0.5;
        //Best word found! Generate the rest recursively.
        out = GenerateReq(lastWrd, bestWrd1, length-oneMap.syllables(oneMap.intToWord[lastWrd]));
        out.push_back(lastWrd);
        int summa = 0;
        for(int i = 0; i < out.size(); i++)
            summa += oneMap.syllables(oneMap.intToWord[out[i]]);
        std::cout << summa << "\t";
        return out;
    }

    std::vector<int> GenerateReq(int lastWrd, int bestWrd, int length)
    {
        //std::cout << "Generation: " << lastWrd << std::endl;
        std::vector<int> out;

        //Break condition - out of syllables
        if (length - oneMap.syllables(oneMap.intToWord[bestWrd]) < 0)
        {
            out.push_back(bestWrd);
            return out;
        }

        //Consider
        int bestWrd2 = 0;
        double maxFreq = 0, tempVal1, tempVal2;
        //std::cout << lastWrd << " " << bestWrd << std::endl;
        std::string temp1 = toString1(lastWrd, bestWrd);
        tempVal2 = mapBigrams[temp1];
        for(int j = 0; j < N; j++)
        {
            //Count occurrences
            //std::vector<int> temp2 = {lastWrd, bestWrd1, j};
            std::string temp2 = toString2(lastWrd,bestWrd,j);
            //int* tempTri =  temp2.data();
            if ((trigrams.find(temp2) != trigrams.end()) && length - oneMap.syllables(oneMap.intToWord[j]))
            {
                //std::cout << "YES\t" << j << "\t" << mapTrigrams[temp2] << std::endl;
                tempVal1 = mapTrigrams[temp2];
            }
            else
                tempVal1 = 0;

            //Check if max
            //std::cout << tempVal1 << "\t" << tempVal2 << std::endl;
            //std::cout << tempVal1/tempVal2 << std::endl;

            if ((tempVal1/tempVal2)*weight[j] > maxFreq)
            {
                bestWrd2 = j;
                maxFreq = (tempVal1/tempVal2)*weight[j];
            }
        }
        //Best word found! Generate next word
        weight[bestWrd] *=0.5;
        out = GenerateReq(bestWrd, bestWrd2, length-oneMap.syllables(oneMap.intToWord[lastWrd]));
        out.push_back(bestWrd);
        return out;
    }
};
