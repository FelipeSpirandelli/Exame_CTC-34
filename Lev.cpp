#include <chrono>
#include <fstream>
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <stdlib.h>
#include <numeric>
#include <algorithm>
#include <conio.h>

#define BACKSPACE 8
#define TAB 9
#define ESCAPE 27
#define DELETE 127

using namespace std;
using namespace chrono;

struct VectorHash {
    size_t operator()(const std::vector<int>& v) const {
        size_t seed = 0;
        for (int i : v) {
            seed ^= std::hash<int>{}(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

class LevenshteinAutomaton {
public:
    string targetWord;
    int max_edits;

    LevenshteinAutomaton(string s, int d) {
        targetWord = s;
        max_edits = d;
    }

    vector<int> start() {
        vector<int> curRow(1 + targetWord.length());
        for (int i = 0; i <= targetWord.length(); i++){ //Initial row (empty input)
            curRow[i] = i;
        }
        return curRow;
    }

    vector<int> NextState(char c, vector<int>& state){
        int i, cost;
        vector<int> newRow = state;
        newRow[0]++;
        
        //change for the dfa
        if (newRow[0] > max_edits) newRow[0] = max_edits + 1;

        for (i = 1; i <= targetWord.length(); i++) {
            cost = (c == targetWord[i-1]) ? 0 : 1 ;
            newRow[i] = min(1 + newRow[i-1], min(1 + state[i], cost + state[i-1]));
            //change the new row to implement DFA
            if (newRow[i] > max_edits) newRow[i] = max_edits + 1;
        }
        return newRow;
    }
    
    bool IsMatch(vector<int>& state) {
        return (state[targetWord.length()] <= max_edits);
    }

    bool CanMatch(vector<int>& state) {
        return (*min_element(state.begin(),state.end()) <= max_edits);
    }

    void PrintRow(vector<int>& state) {
        for (int n : state){
            cout << n << ", ";
        }
        cout << endl;
    }

    unordered_set<char> StateTransitions(vector<int>& state) {
        unordered_set<char> trans;
        for (int i = 0; i < targetWord.length(); i++){
            if (state[i + 1] <= max_edits){
                trans.insert(targetWord[i]);
            }
        }

        trans.insert('*');
        return trans;
    }
};

class Transition{
public:
    int sourceState;
    char c;
    int destState;

    Transition(int i, char j, int k) {
        sourceState = i;
        destState = j;
        c = k;
    }

    pair<int, pair<char, int>> p1; // start, char, end
    pair<char, pair<int, int>> p2; // char, start, end
    pair<pair<int, char>, int> p3;// start, char, end
    vector<pair<char, int>> v; // start = index, char, end
};

class LevenshteinDFA {
public:
    int counter;
    unordered_map<size_t, int> all_states;
    vector<Transition> transitions;
    vector<int> matching;
    LevenshteinAutomaton* automaton;

    LevenshteinDFA(string target, int max_edits) {
        counter = 0;
        automaton = new LevenshteinAutomaton(target, max_edits);
        explore(automaton->start());
    }

    int explore(vector<int> state){
        printTransitions();
        VectorHash hasher;
        size_t key = hasher(state);

        if (all_states.find(key) != all_states.end()) {
            return all_states[key];
        }

        int i = counter;
        counter++;
        all_states[key] = i;
        if (automaton->IsMatch(state)) matching.push_back(i);
        
        for (const char& element : automaton->StateTransitions(state)) {
            vector<int> newState = automaton->NextState(element, state);
            int j = explore(newState);
            transitions.push_back(Transition(i, j, element));
        }
        return i;
    }

    void printMatchingStates(){
        for (int i = 0; i < matching.size(); i++){
            cout << matching[i] << ", ";
        }
        cout << endl << endl;
    }
    void printTransitions(){
        for (int i = 0; i < transitions.size(); i++){
            cout << "Transition from " << transitions[i].sourceState << " to " << transitions[i].destState << " at character " << transitions[i].c << endl;
        }
        cout << endl;
    }
};

int main(){
    string target = "woof";

    LevenshteinDFA levDFA(target,1);
    cout << levDFA.counter;
    //levDFA.printStates();
    /*lev.printRow();
    for (i = 0; i < input.length(); i++){
        lev.NextState(input[i]);
        lev.printRow();
    }*/

    return 0;
}