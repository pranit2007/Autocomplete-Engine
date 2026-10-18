#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <random>
#include "trie.h"
using namespace std;
using Clock = chrono::steady_clock;
static double msSince(Clock::time_point t){
    return chrono::duration<double, milli>(Clock::now() - t).count();
}

static vector<string> baseline(const vector<string>& v,const string& p, int k){
    vector<string> out;
    auto it = lower_bound(v.begin(), v.end(), p);
    for (; it != v.end() && (int)out.size() < k && it->compare(0, p.size(), p) == 0; ++it)
        out.push_back(*it);
    return out;
}

int main(int argc, char** argv){
    string file = argc > 1 ? argv[1] : "words_alpha.txt";
    const int K = 5, Q = 20000;

    vector<string> sorted;
   {
        ifstream in(file);
        string w;
        while (in >> w) sorted.push_back(w);
        sort(sorted.begin(), sorted.end());
        sorted.erase(unique(sorted.begin(), sorted.end()), sorted.end());
    }
    if (sorted.empty()){ cerr<<"Could not read "<<file<<"\n"; return 1; }

    Trie trie(K);
    auto t0 = Clock::now();
    loadWordList(trie, file);
    cout<<"words: "<<sorted.size()<<"   trie nodes: "<<trie.nodeCount()
             <<"   build: "<<msSince(t0)<<" ms   approx memory: "
             <<trie.memoryBytes() / (1024.0 * 1024.0)<<" MB\n";

    mt19937 rng(42);
    vector<string> qs;
    for (int i = 0; i < Q; i++){
        const string& w = sorted[rng() % sorted.size()];
        qs.push_back(w.substr(0, 1 + rng() % min<size_t>(5, w.size())));
    }

    //correctness check
    for (auto& q : qs){
        auto a = baseline(sorted, q, K), b = trie.suggest(q), c = trie.suggestDFS(q, K);
        if (a != b || b != c){ cerr<<"MISMATCH on prefix '"<<q<<"'\n"; return 1; }
    }
    cout<<"correctness: all three methods agree on "<<Q<<" queries\n";

    size_t sink = 0;
    auto run = [&](const char* name, auto&& fn){
        auto t = Clock::now();
        for (auto& q : qs) sink += fn(q).size();
        double ms = msSince(t);
        cout<<name<<": "<<ms<<" ms total, "<<ms * 1000 / Q<<" us/query\n";
    };
    run("sorted vector  ", [&](const string& q){ return baseline(sorted, q, K); });
    run("trie + DFS     ", [&](const string& q){ return trie.suggestDFS(q, K); });
    run("trie + cached  ", [&](const string& q){ return trie.suggest(q); });
    if (sink == 0) cout<<"";
}