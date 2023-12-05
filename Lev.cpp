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
        vector<int> curRow(1 + targetWord.size());
        for (int i = 0; i <= targetWord.size(); i++){ //Initial row (empty input)
            curRow[i] = i;
        }
        return curRow;
    }

    vector<int> NextState(char c, vector<int>& state){
        int i, cost;
        vector<int> newRow(state.size());
        newRow[0] = state[0] + 1;
        
        //change for the dfa
        if (newRow[0] > max_edits) newRow[0] = max_edits + 1;

        for (i = 1; i < state.size(); i++) {
            cost = (c == targetWord[i-1]) ? 0 : 1 ;
            newRow[i] = min(1 + newRow[i-1], min(1 + state[i], cost + state[i-1]));
            //change the new row to implement DFA
            if (newRow[i] > max_edits) newRow[i] = max_edits + 1;
        }

        return newRow;
    }
    
    bool IsMatch(vector<int>& state) {
        return (state.back() <= max_edits);
    }

    bool CanMatch(vector<int>& state) {
        return (*min_element(state.begin(), state.end()) <= max_edits);
    }

    void PrintRow(vector<int>& state) {
        for (int n : state){
            cout << n << ", ";
        }
        cout << endl;
    }

    unordered_set<char> StateTransitions(vector<int>& state) {
        unordered_set<char> trans;
        for (int i = 0; i < targetWord.size(); i++){
            if (state[i] <= max_edits){
                trans.insert(targetWord[i]);
            }
        }

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
    unordered_map<string, int> all_states;
    vector<Transition> transitions;
    vector<unordered_map<char, int>> transitionsVector;
    vector<int> matching;
    LevenshteinAutomaton* automaton;

    LevenshteinDFA(string target, int max_edits) {
        counter = 0;
        automaton = new LevenshteinAutomaton(target, max_edits);
        explore(automaton->start());
    }

    void deleteLev(){
        delete automaton;
    }

    string convertVectorToString(vector<int> state){
        string res;
        for(int val: state)
            res += to_string(val) + ", ";
        return res;
    }

    int explore(vector<int> state){
        // printTransitions();
        string key = convertVectorToString(state);

        if (all_states.find(key) != all_states.end()) {
            return all_states[key];
        }

        int i = counter;
        counter++;
        all_states[key] = i;
        transitionsVector.push_back(unordered_map<char, int>());

        if (automaton->IsMatch(state)) matching.push_back(i);

        for (const char& element : automaton->StateTransitions(state)) {
            //cout << element << endl;
            vector<int> newState = automaton->NextState(element, state);
            int j = explore(newState);
            transitions.push_back(Transition(i, j, element));
            transitionsVector[i][element] = j;
        }
        vector<int> newState = automaton->NextState('*', state);
        int j = explore(newState);
        transitions.push_back(Transition(i, j, '*'));
        transitionsVector[i]['*'] = j;

        return i;
    }

    bool isValidWord(string word){
        int curIndex = 0;
        for(char c: word){
            if(transitionsVector[curIndex].find(c) == transitionsVector[curIndex].end()){
                curIndex = transitionsVector[curIndex]['*'];
            }
            else{
                curIndex = transitionsVector[curIndex][c];
            }
        }

        return std::find(matching.begin(), matching.end(), curIndex) != matching.end();
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

#ifndef EXCLUDE_MAIN
int main(){
    string target = "tests";

     LevenshteinDFA levDFA(target, 1);
     
     cout << levDFA.isValidWord("teste") << endl;
     cout << levDFA.isValidWord("woff") << endl;
     cout << levDFA.isValidWord("wofff") << endl;
     cout << levDFA.isValidWord("aoof") << endl;


     return 0;
}
#endif