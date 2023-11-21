#include <chrono>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;
using namespace std::chrono;

class Node {
public:
    bool isEnd;
    bool isValid;
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

    void constructLev(string word, int distance){
        queue<pair<Node*, pair<int, int>>> lista; // cur node, cur index, cur lev distance
        lista.push({head, {0, 0}});

        while(!lista.empty()){
            auto cur_p_pair = lista.front();
            Node* cur = cur_p_pair.first;
            auto index_dist_pair = cur_p_pair.second;
            int index = index_dist_pair.first;
            int dist = index_dist_pair.second;
            lista.pop();

            cur->isValid = false;
            if(index == word.size()){
                cur->isValid = cur->isEnd; // if end of word and isEnd => valid lev distance
                continue;
            }

            for(auto val: cur->edges){
                if(val.first == word[index])
                    lista.push({val.second, {index + 1, dist}});
                else if(dist < distance)
                    lista.push({val.second, {index + 1, dist + 1}});
            }
        }
    }

    Node* nextNode(Node* cur, char c) {
        if (!cur)
            cur = head;
        Node* next = cur->find(c);
        return next;
    }

    vector<string> next3Words(Node* cur) {
        if (cur == nullptr)
            return {};
        queue<pair<Node*, string>> lista;
        lista.push({cur, ""});
        vector<string> ans;

        while (!lista.empty()) {
            Node* curNode = lista.front().first;
            string curWord = lista.front().second;
            lista.pop();
            if (curNode->isValid) {
                ans.push_back(curWord);
                if (ans.size() == 10)
                    return ans;
            }

            for (auto it : curNode->edges)
                lista.push({it.second, curWord + it.first});
        }
        return ans;
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

    void addLevWord(string s){
        trie.constructLev(s, 1);
    }

    vector<string> handleInput(char c) {
        word.push_back(c);
        addLevWord(word);
        Node* aux = trie.head;
        return trie.next3Words(aux);
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

    while (t--) {
        char c;
        cin >> c;
        cout << c << endl;

        start = high_resolution_clock::now();
        vector<string> suggestedWords = input.handleInput(c);
        stop = high_resolution_clock::now();

        auto durationInside = duration_cast<microseconds>(stop - start);
        cout << "Time taken for handleInput: " << durationInside.count() << " microseconds" << endl;

        cout << "Lev Words: ";
        for (auto& word : suggestedWords)
            cout << word << " ";
        cout << endl << endl;
    }

    return 0;
}
