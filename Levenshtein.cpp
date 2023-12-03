#include <chrono>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <stdlib.h>
#include <numeric>
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
};

class LevTrie  {
public:
    Node* head;

    LevTrie () {
        head = new Node();
    }

    void addWord(string word) {
        Node* aux = head;
        for (auto c : word) {
            Node* next = aux->findOrInclude(c);
            aux = next;
        }
        aux->isEnd = true;
    }


    // TODO - SAVE THE PAST COMPUTATIONS SO THAT WE DONT HAVE TO REDO SEARCH FROM HEAD
    vector<string> search(string word, int maxD){
        vector<int> curRow(word.size() + 1);
        iota (begin(curRow), end(curRow), 0);
        vector<string> ans;

        for(auto val: head->edges){
            searchRecursive(val.second, val.first, string(1, val.first), word, curRow, ans, maxD);
        }

        return ans;
    }

    void searchRecursive(Node* cur, char letter, string curWord, string& word, vector<int> prevRow, vector<string>& ans, int maxD){
        if(ans.size() > 10)
            return;
        vector<int> curRow(word.size() + 1);
        curRow[0] = prevRow[0] + 1;

        for(int i = 1; i <= word.size(); i++){
            int insertCost = curRow[i - 1] + 1;
            int deleteCost = prevRow[i] + 1;
            int replaceCost;

            if(word[i - 1] != letter){
                replaceCost = prevRow[i - 1] + 1;                
            }
            else{
                replaceCost = prevRow[i - 1];
            }

            curRow[i] = min(insertCost, min(deleteCost, replaceCost));
        }

        if(curRow.back() <= maxD && cur->isEnd)
            ans.push_back(curWord);

        if(*min_element(curRow.begin(), curRow.end()) <= maxD){
            for(auto val: cur->edges){
                searchRecursive(val.second, val.first, curWord + val.first, word, curRow, ans, maxD);
            }
        }
    }

    Node* nextNode(Node* cur, char c) {
        if (!cur)
            cur = head;
        Node* next = cur->find(c);
        return next;
    }

    int countNodes(){
        int count = 0;
        queue<Node*> next;
        next.push(head);
        while(!next.empty()){
            Node* curNode = next.front();
            next.pop();
            count++;
            for (auto it : curNode->edges)
                next.push(it.second);
        }
        return count;
    }
};

class Input {
public:
    string word;
    LevTrie& trie;

    Input(LevTrie& trie) : trie(trie) {}

    vector<string> handleInput(char c) {
        word.push_back(c);
        return trie.search(word, 1);
    }
};

int main() {
    LevTrie test;
    Input input(test);

    string line;
    ifstream file("/usr/share/dict/american-english");

    auto start = high_resolution_clock::now();

    if (file.is_open()) {
        while (getline(file, line)) {
            test.addWord(line);
        }
        file.close();
    } else {
        cerr << "Unable to open file" << endl;
        return 1;
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken to insert words: " << duration.count() << " miliseconds" << endl;

    int nNodes = test.countNodes();
    cout << "Number of nodes: " << nNodes << endl << "Memory: " << ((double) nNodes * sizeof(Node) / 1048576.0)<< " MB" << endl;

    int t;
    cout << endl << "Digite quantas letras quer" << endl;
    cin >> t;
    cout << endl << "Digite as letras: " << endl << endl;

    while (t--) {
        char c;
        cin >> c;

        start = high_resolution_clock::now();
        vector<string> suggestedWords = input.handleInput(c);
        stop = high_resolution_clock::now();

        auto durationInside = duration_cast<microseconds>(stop - start);
        cout << "Time taken for handleInput: " << durationInside.count() << " microseconds" << endl;

        cout << "Lev Words for word '" << input.word << "': ";
        for (auto& word : suggestedWords)
            cout << word << " ";
        cout << endl << endl;
    }

    return 0;
}
