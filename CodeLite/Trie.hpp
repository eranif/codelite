#ifndef TRIE_HPP
#define TRIE_HPP

#include <unordered_map>
#include <vector>
#include <wx/string.h>

class TrieTree;
typedef TrieTree TreeNode;
class TrieTree
{
    unordered_map<wxChar, TrieTree*> m_children;
    bool end_of_sequence = false;

public:
    bool is_exact_match() const { return this->end_of_sequence; }

    ~TrieTree()
    {
        for(auto& vt : m_children) {
            delete vt.second;
        }
        m_children.clear();
    }

    /**
     * @brief insert a sequence into the tree
     */
    void insert(const wxString& sequence)
    {
        TrieTree* cur = this;
        for(const wxChar& d : sequence) {
            if(cur->m_children.count(d) == 0) {
                cur->m_children.insert({ d, new TrieTree() });
            }
            cur = cur->m_children[d];
        }

        // mark the last node as a "end of sequence"
        cur->end_of_sequence = true;
    }

    /**
     * @brief return if the tree contains a given sequence
     */
    const TreeNode* contains(const wxString& sequence) const
    {
        const TrieTree* cur = this;
        for(const auto& d : sequence) {
            if(cur->m_children.count(d) == 0) {
                return nullptr;
            }
            cur = cur->m_children.find(d)->second;
        }

        // if we reached here, it means that we were able to match the entire
        // sequence, however, we only return true if this entire sequence was
        // inserted and its not a sub sequence of something else
        return cur;
    }
};
#endif // TRIE_HPP
