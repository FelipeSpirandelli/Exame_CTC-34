#define EXCLUDE_MAIN
#include "FST.cpp"
#undef EXCLUDE_MAIN
#include <stdio.h>

int main(){
    const int MAX_WORD_SIZE = 100;

    unordered_set<Node*, hash<Node*>, NodePtrEqual> dictionary;
    vector<Node*> tempStates(MAX_WORD_SIZE);
    for(int i = 0; i < MAX_WORD_SIZE; i++)
        tempStates[i] = new Node();
    string prevWord = "";
    
    string curWord;
    ifstream file(DICTPATH);
    // ifstream file("./test.txt");

    vector<string> all_words;

    // read input
    if (file.is_open()) {
        // read each word
        while (getline(file, curWord)) {
            all_words.push_back(curWord);
        }
    } else {
        cerr << "Unable to open file" << endl;
        return 1;
    }
    sort(all_words.begin(), all_words.end());

    time_point start = high_resolution_clock::now(); //Considering setup begins after the sorting

    for(string curWord: all_words){
        // get prefix size
        int i = 1;
        while(i <= curWord.size() && i <= prevWord.size() && curWord[i - 1] == prevWord[i - 1]){
            i++;
        }
        int prefixLengthPlusOne = i;

        // minimize states from prefix of prevWord
        for(i = prevWord.size(); i >= prefixLengthPlusOne; i--){
            tempStates[i - 1]->setTransition(prevWord[i - 1], findMinimized(tempStates[i], dictionary));
        }

        // tail states for curWord
        for(int i = prefixLengthPlusOne; i <= curWord.size(); i++){
            tempStates[i] = new Node();
            tempStates[i - 1]->setTransition(curWord[i - 1], tempStates[i]);
        }

        if(curWord != prevWord){
            tempStates[curWord.size()]->isEnd = true;
        }

        prevWord = curWord;
    }
    for(int i = all_words.back().size() - 1; i >= 1; i--){
        tempStates[i - 1]->setTransition(prevWord[i], findMinimized(tempStates[i], dictionary));
    }

    time_point stop = high_resolution_clock::now();
    duration<double,micro> buildTime = duration_cast<microseconds>(stop - start);
    cout << "Time taken to create FST: " << buildTime.count() << " microseconds" << endl;
    Fst fst(findMinimized(tempStates[0], dictionary));
   double memory = ((double) dictionary.size() * sizeof(Node) / 1048576.0);
    cout << "Memory used by the FST: " << memory << "MB" << endl;

    //Start the query tests
    double averageTimes[24] = {0};
    int counts[24] = {0};
    file.clear();
    file.seekg(0,ios::beg);
    duration<double, micro> duration;
    while (getline(file,curWord)){
         start = high_resolution_clock::now(); 
        //cout << "Word" << word << ", Size " << word.size();
        fst.next3Words(fst.head, curWord); //Tempo para o autocomplete
        stop = high_resolution_clock::now();
        duration = stop - start;
        averageTimes[curWord.size()] += duration.count();
        counts[curWord.size()] += 1;
    }
    file.close();

    for (int i = 1; i < 24; i++){
        averageTimes[i] = averageTimes[i]/counts[i];
        cout << "Number of words with " << i << " letters: "  << counts[i] << ". Average time (us): " << averageTimes[i] << endl;
    }

    ofstream outFile("fstData.csv");
    if (outFile.is_open()){
        for (int i = 1; i < 24; i++){
            outFile << averageTimes[i];
            outFile << ",";
            outFile << counts[i];
            outFile << ",";
            if (i == 1){ //Also write the other statistics
                outFile << dictionary.size(); //num of nodes
                outFile << ",";
                outFile << buildTime.count(); //Time spent on creation of FST (us)
                outFile << ",";
                outFile << memory; //size of Trie in MB
                outFile << ",";
            }
            outFile << endl;
        }
        cout << sizeof(unordered_map<char, Node*>);
    }
    else {
        cerr << "Unable to open file for writing" << endl;

    }
    return 0;
}