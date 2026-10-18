#include <chrono>
#include <iostream>
#include "trie.h"
using namespace std;
int main(int argc, char** argv) {
    string file = argc > 1 ? argv[1] : "words_alpha.txt";
    Trie trie(5);

    auto t0 = chrono::steady_clock::now();
    size_t n = loadWordList(trie, file);
    double ms = chrono::duration<double, milli>(
                    chrono::steady_clock::now() - t0).count();
    if (n == 0) { cerr<<"Could not load words from "<<file<<"\n"; return 1; }
    cout<<"Loaded "<<n<<" words ("<<trie.nodeCount()<<" nodes) in "
             <<ms<<" ms\n";
    cout<<"Type a prefix for suggestions.  ':pick <word>' boosts a word.  ':q' quits.\n";

    string line;
    while (cout<<"> " && getline(cin, line)) {
        if (line == ":q") break;
        if (line.rfind(":pick ", 0) == 0) {
            string w = line.substr(6);
            cout<<(trie.bump(w, 100) ? "boosted " + w : "unknown word: " + w)<<"\n";
            continue;
        }
        auto t1 = chrono::steady_clock::now();
        auto res = trie.suggest(line);
        double us = chrono::duration<double, micro>(
                        chrono::steady_clock::now() - t1).count();
        if (res.empty()) cout<<"  (no suggestions)\n";
        for (auto& s : res) cout<<"  "<<s<<"\n";
        cout<<"  ["<<us<<" us]\n";
    }
}