class TrieNode:
    def __init__(self):
        self.children = {}
        self.is_end = False

class AutocompleteEngine:
    def __init__(self):
        self.root = TrieNode()
    def insert(self, word: str) -> None:
        node = self.root
        for char in word:
            if char not in node.children:
                node.children[char] = TrieNode()
            node = node.children[char]
        node.is_end = True
    def findnode(self, prefix: str) -> TrieNode:
        node = self.root
        for char in prefix:
            if char not in node.children:
                return None
            node = node.children[char]
        return node
    def _dfs(self, node: TrieNode, prefix: str, suggestions: list) -> None:
        if node.is_end:
            suggestions.append(prefix)
        for char, child_node in node.children.items():
            self._dfs(child_node, prefix + char, suggestions)
    def get_suggestions(self, prefix: str) -> list:
        suggestions = []
        node = self.findnode(prefix)
        if not node:
            return suggestions    
        self._dfs(node, prefix, suggestions)
        return suggestions

if __name__ == "__main__":
    engine = AutocompleteEngine()
    vocabulary = ["autocomplete", "automatic", "automobile", "apple", "application", "aptitude"]
    for word in vocabulary:
        engine.insert(word)
    inp = "auto"
    results = engine.get_suggestions(inp)
    print(f"Suggestions for '{inp}': {results}")