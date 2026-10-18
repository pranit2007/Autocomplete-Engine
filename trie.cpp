#include "trie.h"
#include <vector>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
using namespace std;
Trie::Trie(int k) : K(k){ nodes.emplace_back(); }

bool Trie::better(int a, int b) const{
    if(freqs[a] != freqs[b]) return freqs[a] > freqs[b];
    return words[a] < words[b];
}

void Trie::updateTop(Node& n, int id){
    auto& v = n.top;
    auto it = std::find(v.begin(), v.end(), id);
    if(it != v.end()) v.erase(it);
    auto pos = lower_bound(v.begin(), v.end(), id,
                                [&](int a, int b){ return better(a, b); });
    v.insert(pos, id);
    if((int)v.size() > K) v.pop_back();
}

void Trie::refreshPath(const vector<int32_t>& path, int id){
    for(int u : path) updateTop(nodes[u], id);
}

bool Trie::insert(const string& w, uint64_t f){
    if(w.empty()) return false;
    for(char ch : w) if(ch < 'a' || ch > 'z') return false;

    vector<int32_t> path{0};
    int cur = 0;
    for(char ch : w){
        int c = ch - 'a';
        if(nodes[cur].next[c] == -1){
            int idx =(int)nodes.size();
            nodes.emplace_back();
            nodes[cur].next[c] = idx;
        }
        cur = nodes[cur].next[c];
        path.push_back(cur);
    }
    if(nodes[cur].wordId == -1){
        nodes[cur].wordId =(int)words.size();
        words.push_back(w);
        freqs.push_back(0);
    }
    int id = nodes[cur].wordId;
    freqs[id] += f;
    refreshPath(path, id);
    return true;
}

bool Trie::bump(const string& w, uint64_t by){
    vector<int32_t> path{0};
    int cur = 0;
    for(char ch : w){
        if(ch < 'a' || ch > 'z') return false;
        cur = nodes[cur].next[ch - 'a'];
        if(cur == -1) return false;
        path.push_back(cur);
    }
    int id = nodes[cur].wordId;
    if(id == -1) return false;
    freqs[id] += by;
    refreshPath(path, id);
    return true;
}

int Trie::find(const string& s) const{
    int cur = 0;
    for(char ch : s){
        if(ch < 'a' || ch > 'z') return -1;
        cur = nodes[cur].next[ch - 'a'];
        if(cur == -1) return -1;
    }
    return cur;
}

vector<string> Trie::suggest(const string& prefix) const{
    int u = find(prefix);
    vector<string> out;
    if(u == -1) return out;
    for(int id : nodes[u].top) out.push_back(words[id]);
    return out;
}

void Trie::dfs(int u, vector<int32_t>& out) const{
    if(nodes[u].wordId != -1) out.push_back(nodes[u].wordId);
    for(int c = 0; c < 26; c++)
        if(nodes[u].next[c] != -1) dfs(nodes[u].next[c], out);
}

vector<string> Trie::suggestDFS(const string& prefix, int k) const{
    int u = find(prefix);
    vector<string> res;
    if(u == -1) return res;
    vector<int32_t> ids;
    dfs(u, ids);
    k = min<int>(k, ids.size());
    partial_sort(ids.begin(), ids.begin() + k, ids.end(),[&](int a, int b){ return better(a, b); });
    for(int i = 0; i < k; i++) res.push_back(words[ids[i]]);
    return res;
}

size_t Trie::memoryBytes() const{
    size_t b = nodes.capacity() * sizeof(Node);
    for(const auto& n : nodes) b += n.top.capacity() * sizeof(int32_t);
    for(const auto& w : words) b += sizeof(string) + w.capacity();
    b += freqs.capacity() * sizeof(uint64_t);
    return b;
}

size_t loadWordList(Trie& trie, const string& path){
    ifstream in(path);
    string line;
    size_t n = 0;
    while(getline(in, line)){
        istringstream ss(line);
        string w;
        uint64_t f = 1;
        if(!(ss >> w)) continue;
        if(!(ss >> f)) f = 1;
        transform(w.begin(), w.end(), w.begin(),[](unsigned char c){ return tolower(c); });
        if(trie.insert(w, f)) n++;
    }
    return n;
}