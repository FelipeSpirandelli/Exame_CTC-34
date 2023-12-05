#include "Lev.cpp"
#include "FST.cpp"
const int MAX_WORD_SIZE = 100;

class FSTLev
{
public:
    vector<Node *> tempStates;
    unordered_set<Node *, hash<Node *>, NodePtrEqual> dictionary;
    Node *head;

    FSTLev(vector<string> &all_words)
    {
        tempStates = vector<Node *>(MAX_WORD_SIZE);
        for (int i = 0; i < MAX_WORD_SIZE; i++)
            tempStates[i] = new Node();
        buildFST(all_words);
        head = findMinimized(tempStates[0]);
    }

    Node *findMinimized(Node *cur)
    {
        if (dictionary.find(cur) != dictionary.end())
        {
            return *dictionary.find(cur);
        }
        dictionary.insert(cur);
        return cur;
    }

    void buildFST(vector<string> &all_words)
    {

        string prevWord = "";

        auto start = high_resolution_clock::now();
        for (string curWord : all_words)
        {
            // get prefix size
            int i = 1;
            while (i <= curWord.size() && i <= prevWord.size() && curWord[i - 1] == prevWord[i - 1])
            {
                i++;
            }
            int prefixLengthPlusOne = i;

            // minimize states from prefix of prevWord
            for (i = prevWord.size(); i >= prefixLengthPlusOne; i--)
            {
                tempStates[i - 1]->setTransition(prevWord[i - 1], findMinimized(tempStates[i]));
            }

            // tail states for curWord
            for (int i = prefixLengthPlusOne; i <= curWord.size(); i++)
            {
                tempStates[i] = new Node();
                tempStates[i - 1]->setTransition(curWord[i - 1], tempStates[i]);
            }

            if (curWord != prevWord)
            {
                tempStates[curWord.size()]->isEnd = true;
            }

            prevWord = curWord;
        }
        for (int i = all_words.back().size() - 1; i >= 1; i--)
        {
            tempStates[i - 1]->setTransition(prevWord[i], findMinimized(tempStates[i]));
        }

        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop - start);

        double memory = ((double)dictionary.size() * sizeof(Node) / 1048576.0);
        cout << "Time taken to create FST: " << duration.count() << " miliseconds" << endl
             << "Memory: " << memory << " MB" << endl;
    }

    Node *nextNode(Node *cur, char c)
    {
        if (!cur)
            return cur;

        Node *next = cur->find(c);
        return next;
    }

    vector<string> next3Words(Node *cur, string word)
    {
        if (cur == nullptr)
            return {};
        LevenshteinDFA levDFA(word, 1);

        queue<pair<Node *, string>> words;
        words.push({cur, word});
        vector<string> ans;

        while (!words.empty())
        {
            Node *curNode = words.front().first;
            string curWord = words.front().second;
            words.pop();
            if (curNode->isEnd && curWord != word && levDFA.isValidWord(curWord))
            {
                ans.push_back(curWord);
                if (ans.size() == 3)
                    return ans;
            }

            for (pair<const char, Node *> it : curNode->edges)
                words.push({it.second, curWord + it.first});
        }

        levDFA.deleteLev();
        return ans;
    }
};

class InputFSTLev
{
public:
    string word;
    FSTLev &fst;
    stack<Node *> nodes;

    InputFSTLev(FSTLev &fst) : fst(fst) {}

    vector<string> handleInput(char c)
    {
        if (c == BACKSPACE || c == DELETE)
        {
            if (!nodes.empty())
            {
                nodes.pop();
                word.pop_back();
            }
            if (!nodes.empty())
                return fst.next3Words(nodes.top(), word);
        }
        else
        {
            if (c == TAB && !word.empty())
            {
                vector<string> words = fst.next3Words(nodes.top(), word);

                if (words.size() == 0)
                    return {};

                string next_word = words[0];
                for (int i = word.length(); i < next_word.length(); i++)
                    nodes.push(fst.nextNode(nodes.top(), next_word[i]));
                word = next_word;

                return fst.next3Words(nodes.top(), word);
            }

            Node *next = (nodes.empty()) ? fst.head : nodes.top();
            nodes.push(fst.nextNode(next, c));
            word.push_back(c);
            return fst.next3Words(nodes.top(), word);
        }
        return {};
    }
};

int main()
{

    string curWord;
    ifstream file("/usr/share/dict/american-english");
    // ifstream file("./test.txt");


    vector<string> all_words;
    // read input
    if (file.is_open())
    {
        // read each word
        while (getline(file, curWord))
        {
            all_words.push_back(curWord);
        }
    }
    else
    {
        cerr << "Unable to open file" << endl;
        return 1;
    }
    sort(all_words.begin(), all_words.end());

    FSTLev fst(all_words);
    cout << "Press any key to start" << endl;
    getch();
    InputFSTLev input(fst);
    char input_letter = 0;

    while (input_letter != ESCAPE)
    {
        system("clear");

        auto start = high_resolution_clock::now();
        vector<string> suggestedWords = input.handleInput(input_letter);
        auto stop = high_resolution_clock::now();

        cout << u8"\u2192 " << input.word << endl;

        if (input_letter != 0)
        {
            for (std::basic_string<char> &word : suggestedWords)
                cout << "  " << word << endl;

            auto durationInside = duration_cast<microseconds>(stop - start);
            cout << endl
                 << "Time taken for handleInput: " << durationInside.count() << " microseconds";
        }

        input_letter = getch();
    }

    return 0;
}
