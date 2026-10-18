#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>
using namespace std;

class Trie{
public:
    explicit Trie(int k = 5);

    //Adds freq to the word's score (creates the word if new), returns false if the word has characters outside a-z.
    bool insert(const string& word, uint64_t freq = 1);

    bool bump(const string& word, uint64_t by = 1);

    //walk to the prefix node, return its cached top-k
    vector<string> suggest(const string& prefix) const;

    //DFS the subtree, then sort.
    vector<string> suggestDFS(const string& prefix, int k) const;

    size_t nodeCount() const{ return nodes.size();}
    size_t wordCount() const{ return words.size();}
    size_t memoryBytes() const;

private:
    struct Node{
        array<int32_t, 26> next;
        int32_t wordId = -1; //-1 = no word ends here
        vector<int32_t> top;
        Node(){ next.fill(-1);}
   };

    int K;
    vector<Node> nodes;
    vector<string> words;
    vector<uint64_t> freqs;

    bool better(int a, int b) const;
    void updateTop(Node& n, int id);
    void refreshPath(const vector<int32_t>& path, int id);
    int find(const string& s) const;
    void dfs(int u, vector<int32_t>& out) const;
};

size_t loadWordList(Trie& trie, const string& path);