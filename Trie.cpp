#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>
#include <conio.h>

#define BACKSPACE 8
#define TAB 9
#define DELETE 127
#define DICTPATH "./american-english" //Mudar pra /usr/share/dict/american-english

using namespace std;
using namespace std::chrono;
namespace TrieNamespace {
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
    size_t getSize(){
        size_t size = sizeof(Node);
        for (const auto& pair : edges){
            if (pair.second != nullptr){
                size += sizeof(char) + sizeof(Node*) + pair.second->getSize();
            }
        }
        return size;
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

    size_t getSize() const {
        if (head != nullptr){
            return sizeof(Trie) + head->getSize();
        }
        return 0;
    }
    Node* nextNode(Node* cur, char c) {
        if (!cur)
            cur = head;

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
        if (c == BACKSPACE || c == DELETE) {
            if (!nodes.empty()) {
                nodes.pop();
                word.pop_back();
            }
            if (!nodes.empty())
                return trie.next3Words(nodes.top(), word);
        } else {
            // if (c == TAB && !word.empty()) {
            //     Node* next = (nodes.empty()) ? trie.head : nodes.top();
            //     vector<string> words = trie.next3Words(nodes.top(), word);

            //     if (word == words[0])
            //         return words;

            //     string next_word = words[0];
            //     for (int i = next_word.length() - word.length() - 1; i < next_word.length(); i++)
            //         nodes.push(trie.nextNode(next, next_word[i]));

            //     word = next_word;

            //     return trie.next3Words(nodes.top(), word);
            // }

            Node* next = (nodes.empty()) ? trie.head : nodes.top();
            nodes.push(trie.nextNode(next, c));
            word.push_back(c);
            return trie.next3Words(nodes.top(), word);
        }
        return {};
    }
};

#ifndef EXCLUDE_MAIN
int main() {
    Trie trie;
    Input input(trie);

    string line;
    ifstream file(DICTPATH);
    // ifstream file("./test.txt");

    time_point start = high_resolution_clock::now();

    if (file.is_open()) {
        while (getline(file, line)) {
            trie.addWord(line);
        }
        file.close();
    } else {
        cerr << "Unable to open file" << endl;
        return 1;
    }

    time_point stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "Time taken to insert words: " << duration.count() << " miliseconds" << endl;

    int nNodes = trie.countNodes();
    cout << "Number of nodes: " << nNodes << endl << "Memory: " << (trie.getSize() / 1048576.0)/*((double) nNodes * sizeof(Node) / 1048576.0)*/<< " MB" << endl;

    cout << "Press any key to start" << endl;
    getch();

    char input_letter = 0;
    while (input_letter != '\n') {
        system("clear");

        start = high_resolution_clock::now();
        vector<string> suggestedWords = input.handleInput(input_letter);
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
#endif
}