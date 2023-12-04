#include <chrono>
#include <fstream>
#include <queue>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <conio.h>

#define BACKSPACE 8
#define DELETE 127

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
    for (pair<const char, Node *> val : cur->edges) {
        curWord += val.first;
        dfs(curWord, val.second);
        curWord.pop_back();
    }
}


Node* findMinimized(Node* cur, unordered_set<Node*, hash<Node*>, NodePtrEqual>& dictionary){
    if(dictionary.find(cur) != dictionary.end()){
        return *dictionary.find(cur);
    }
    dictionary.insert(cur);
    return cur;
}


vector<string> next3Words(Node* cur, unordered_set<Node*, hash<Node*>, NodePtrEqual> &dictionary, string word) {
    queue<pair<Node*, string>> words;
    words.push({cur, word});
    vector<string> ans;

    while (!words.empty()) {
        Node* curNode = words.front().first;
        string curWord = words.front().second;
        words.pop();
        if (curNode->isEnd) {
            ans.push_back(curWord);
            if (ans.size() == 5)
                    return ans;
        }

        auto newNode = findMinimized(curNode, dictionary);
        for (pair<const char, Node *> it : curNode->edges)
            words.push({it.second, curWord + it.first});
    }

    return ans;
}

class Input {
public:
    string word;
    unordered_set<Node*, hash<Node*>, NodePtrEqual> dictionary;

    Input(unordered_set<Node*, hash<Node*>, NodePtrEqual> &dictionary) : dictionary(dictionary) {}

    vector<string> handleInput(Node* cur, char c) {
        if (c == BACKSPACE || c == DELETE) {
            if (!word.empty())
                word.pop_back();

            if (!word.empty())
                return next3Words(cur, dictionary, word);
        } else {
            word.push_back(c);
            return next3Words(cur, dictionary, word);
        }
        return {};
    }
};


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

    auto start = high_resolution_clock::now();

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

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    double memory = ((double) dictionary.size() * sizeof(Node) / 1048576.0);
    cout << "Time taken to create FST: " << duration.count() << " miliseconds" << endl << "Memory: " << memory << " MB" << endl;

    cout << "Press any key to start" << endl;
    getch();

    Input input(dictionary);
    char input_letter = 0;
    Node* initial = findMinimized(tempStates[0], dictionary);

    while (input_letter != '\n') {
        system("clear");

        start = high_resolution_clock::now();
        vector<string> suggestedWords = input.handleInput(initial, input_letter);
        stop = high_resolution_clock::now();

        cout << u8"\u2192 " << input.word << endl;

        if (input_letter != 0) {
            // cout << "Avaliable words:" << endl;
            for (std::basic_string<char>& word : suggestedWords)
                cout << "  " << word << endl;

            auto durationInside = duration_cast<microseconds>(stop - start);
            cout << endl <<  "Time taken for handleInput: " << durationInside.count() << " microseconds";
        }

        input_letter = getch();
    }

    return 0;
}
