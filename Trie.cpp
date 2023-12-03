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
    unordered_map<char, Node*> edges;

    Node() { isEnd = false; }

    Node* find(char c) {
        unordered_map<char, Node*>::iterator it = edges.find(c);
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

class Trie {
public:
    Node* head;
    Trie() { head = new Node(); }

    void addWord(string word) {
        Node* aux = head;
        for (char c : word) {
            Node* next = aux->findOrInclude(c);
            aux = next;
        }
        aux->isEnd = true;
    }

    Node* nextNode(Node* cur, char c) {
        if (!cur) cur = head;

        Node* next = cur->find(c);
        return next;
    }

    vector<string> next3Words(Node* cur, string word) {
        if (cur == nullptr) 
            return {};

        queue<pair<Node*, string>> words;
        words.push({cur, word});
        vector<string> ans;

        while (!words.empty()) {
            Node* curNode = words.front().first;
            string curWord = words.front().second;
            words.pop();
            if (curNode->isEnd) {
                ans.push_back(curWord);
                if (ans.size() == 3)
                    return ans;
            }

            for (pair<const char, Node *> it : curNode->edges)
                words.push({it.second, curWord + it.first});
        }
        return ans;
    }

    int countNodes(){
        int count = 0;
        queue<Node*> next;
        next.push(head);

        while(!next.empty()) {
            Node* curNode = next.front();
            next.pop();
            count++;
            for (pair<const char, Node *> it : curNode->edges)
                next.push(it.second);
        }
        return count;
    }
};

class Input {
public:
    string word;
    Trie& trie;
    stack<Node*> nodes;

    Input(Trie& trie) : trie(trie) {}

    vector<string> handleInput(char c) {
        if (c == 8) { // backspace
            if (!nodes.empty()) {
                nodes.pop();
                word.pop_back();
            }
            if (!nodes.empty())
                return trie.next3Words(nodes.top(), word);
        } else {
            Node* next = (nodes.empty()) ? trie.head : nodes.top();
            nodes.push(trie.nextNode(next, c));
            word.push_back(c);
            return trie.next3Words(nodes.top(), word);
        }
        return {};
    }
};

int main() {
    Trie test;
    Input input(test);

    string line;
    ifstream file("/usr/share/dict/american-english");
    // ifstream file("./test.txt");

    time_point start = high_resolution_clock::now();

    if (file.is_open()) {
        while (getline(file, line)) {
            test.addWord(line);
        }
        file.close();
    } else {
        cerr << "Unable to open file" << endl;
        return 1;
    }

    time_point stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken to insert words: " << duration.count() << " miliseconds" << endl;

    int nNodes = test.countNodes();
    cout << "Number of nodes: " << nNodes << endl << "Memory: " << ((double) nNodes * sizeof(Node) / 1048576.0)<< " MB" << endl;

    int t;
    cout << endl << "How many letters has the word?" << endl;
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

        for (std::basic_string<char>& word : suggestedWords)
            cout << word << " ";
        cout << endl;
    }

    return 0;
}
