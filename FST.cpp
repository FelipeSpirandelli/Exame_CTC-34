#include <chrono>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
using namespace std;
using namespace std::chrono;

class Node {
public:
    bool isEnd;
    unordered_map<char, Node*> edges;

    Node() {
        isEnd = false;
    }

    Node* find(char c) {
        auto it = edges.find(c);
        if (it != edges.end())
            return it->second;

        return nullptr;
    }

    Node* findOrInclude(char c) {
        if (edges.find(c) == edges.end())
            edges[c] = new Node();

        return edges[c];
    }

    void setTransition(char c, Node* next){
        edges[c] = next;
    }

    void clearState(){
        edges.clear();
        isEnd = true;
    }

    bool operator==(const Node& other) const {
        return (edges == other.edges);
    }
};

namespace std {
    template <>
    struct hash<Node*> {
        size_t operator()(const Node* node) const {
            size_t edges_hash = 0;

            for (const auto& edge : node->edges) {
                edges_hash ^= hash<char>()(edge.first) 
                                    ^ (hash<Node*>()(edge.second) << 1);
            }

            return edges_hash;
        }
    };
};

struct NodePtrEqual {
    bool operator()(const Node* lhs, const Node* rhs) const {
        return *lhs == *rhs; // Calls Node::operator==
    }
};

void dfs(string& curWord, Node* cur){
    if(cur->isEnd)
        cout << curWord << endl;
    for(auto val: cur->edges){
        curWord += val.first;
        dfs(curWord, val.second);
        curWord.pop_back();
    }
}


Node* findMinimized(Node* cur, unordered_set<Node*, hash<Node*>, NodePtrEqual>& dictionary){
    if(dictionary.find(cur) != dictionary.end()){
        // cout << "worked" << endl;
        return *dictionary.find(cur);
    }
    dictionary.insert(cur);
    return cur;
}


int main(){

    const int MAX_WORD_SIZE = 100;

    unordered_set<Node*, hash<Node*>, NodePtrEqual> dictionary;
    vector<Node*> tempStates(MAX_WORD_SIZE);
    for(int i = 0; i < MAX_WORD_SIZE; i++)
        tempStates[i] = new Node();
    string prevWord = "";
    
    string curWord;
    ifstream file("/usr/share/dict/american-english");
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


    for(int i = 0; i < 5; i++)
        cout << all_words[i] << endl;

    auto start = high_resolution_clock::now();

    for(string curWord: all_words){
        // get prefix size
        int i = 1;
        while(i <= curWord.size() && i <= prevWord.size() && curWord[i - 1] == prevWord[i - 1]){
            i++;
        }
        int prefixLengthPlusOne = i;
        
        // cout << "Line " << prevWord << " " << curWord << " " << prefixLengthPlusOne << endl;

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

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken to create FST: " << duration.count() << " miliseconds" << endl;

    Node* initial = findMinimized(tempStates[0], dictionary);

    // cout << endl << "start" << endl;
    // for(auto val: initial->edges){
    //     cout << val.first << endl;
    // }
    // cout << "end" << endl;

    // cout << endl << endl << "DFS" << endl;
    // string aux = "";
    // dfs(aux, initial);

    cout << dictionary.size() << endl;

    return 0;
}