#include <iostream>
#include "HMM.hpp"
#include <vector>
#include <fstream>
#include <cstring>
#include <sstream>
#include <unordered_map>
#include <cmath>

//std::unordered_set<std::string> wordset;
std::unordered_map<std::string,int> wordmap;
std::vector<std::string> words;
std::vector<int> index;
int maxIndex = -1;

bool inVector(std::string in);
int indexInVector(std::string in);
std::string getWord(int i);


bool inVector(std::string in)
{
    return wordmap.find(in) != wordmap.end();
}

int indexInVector(std::string in)
{
    return wordmap[in];
}

std::string getWord(int i)
{
    for(int j=0;j<(int)index.size();++j)
    {
        if(i == index[j])
            return words[j];
    }
    return "";
}

int main()
{
    std::string line;
    //std::ifstream myfile ("poems.txt");
    std::ifstream myfile("ShakespeareSonnets.txt");

    std::vector<int>sequence;

    if (myfile.is_open())
    {
        /* Skip copyright notices etcetera */
        for(int i = 0; i < 288; ++i) getline(myfile,line);
        /* Actual used code*/
        int nope = 0;
        while ( getline (myfile,line) && nope <= 100)//&& line != "End of The Project Gutenberg Etext of Shakespeare's Sonnets")
        {
            nope++;
            if (line.length() >= 10)
            {
                //std::cout << line << std::endl;
                //spara ord + Baumwelch + add
                //std::vector<int>sequence;
                std::istringstream iss;
                iss.str(line);
                while (!iss.eof())
                {
                    std::string temp;
                    iss >> temp;
                    //std::cerr << temp << std::endl;
                    if(!inVector(temp))
                    {
                        maxIndex++;
                        wordmap[temp] = maxIndex;
                        words.push_back(temp);
                        index.push_back(maxIndex);
                        sequence.push_back(maxIndex);
                    }
                    else
                    {
                        sequence.push_back(indexInVector(temp));
                    }
                }
                //sequences.push_back(sequence);
            }
        }
        myfile.close();
    }
    else std::cout << "Unable to open file";
    std::cout << "Reading done" << std::endl;
    HMM model(maxIndex+1,maxIndex+1);
    model.reset();

    std::cerr << "sequence.size(): "<< sequence.size() << "  " << std::endl;

    std::cerr << "Baum-Welch" << std::endl;
    model.BaumWelch(sequence);
    model.add();
    std::cerr << "Baum-Welch + add done" << std::endl;

    for(int j=0; j<8; j++) //multiple sentence
    {
//        std::vector<int> test(15, 1); //decide size of sentence
//        for (int i=0; i<test.size(); i++)
//        {
//            test[i] = (100*rand())%maxIndex;
//        }
        //std::vector<int> ny = model.Viterbi(test);
        std::vector<int> ny = model.Generate(j,10);//model.Viterbi(test);
        //std::vector<int> ny = model.Viterbi(ny1);

        for(int i=0;i<(int)ny.size();++i)
            std::cerr << getWord(ny[i]) << " ";
        std::cerr << "\n" << std::endl;
    }

	return 0;
}
