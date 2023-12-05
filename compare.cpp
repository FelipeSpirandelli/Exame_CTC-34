#define EXCLUDE_MAIN
#include "Trie.cpp"
#undef EXCLUDE_MAIN
#include <stdio.h>
int main(){
    TrieNamespace::Trie trie;
    ifstream file(DICTPATH);
    string word;
    double averageTimes[100] = {0.0};
    int counts[24] = {0};
    double indexCreateTime;
    time_point start = high_resolution_clock::now(); 
    time_point stop = high_resolution_clock::now();
    duration<double, micro> duration = stop - start;
    int i;

    if (file.is_open()){
        //build the trie
        start = high_resolution_clock::now();
        while (getline(file, word)) {
            trie.addWord(word);
        }
        stop = high_resolution_clock::now();
        duration = stop - start;
        indexCreateTime = duration.count();
        file.clear();
        file.seekg(0,ios::beg);
    
        while (getline(file,word)){
            start = high_resolution_clock::now(); 
            //cout << "Word" << word << ", Size " << word.size();
            trie.next3Words(trie.head, word); //Tempo para o autocomplete
            stop = high_resolution_clock::now();
            duration = stop - start;

            averageTimes[word.size()] += duration.count();
            counts[word.size()] += 1;
        }
        file.close();
        //fflush(stdin);
        //scanf("%d",&i);
         //Print all
        for (i = 1; i < 24; i++){
            averageTimes[i] = averageTimes[i]/counts[i];
            cout << "Number of words with " << i << " letters: "  << counts[i] << ". Average time (us): " << averageTimes[i] << endl;
        }

        //Save to csv
        ofstream outFile("trieData.csv");
        if (outFile.is_open()){
            for (i = 1; i < 24; i++){
                outFile << averageTimes[i];
                outFile << ",";
                outFile << counts[i];
                outFile << ",";
                if (i == 1){ //Also write the other statistics
                    outFile << trie.countNodes(); //num of nodes
                    outFile << ",";
                    outFile << indexCreateTime; //Time of creation of index (us)
                    outFile << ",";
                    outFile << trie.getSize() / 1048576.0; //size in MB
                    outFile << ",";
                }
                outFile << endl;

            }
        }
        else {
            cerr << "Unable to open file for writing" << endl;
        }
        
    }
    else {
        cerr << "Arquivo american-english não encontrado" << endl;
    }
    return 0;
}

