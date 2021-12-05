#ifndef TRIE_HPP
#define TRIE_HPP

#include <unordered_map>
#include <vector>

using namespace std;
template <typename T> class clTrieNode
{
    unordered_map<T, clTrieNode*> m_children;
    bool end_of_sequence = false;

public:
    /**
     * @brief insert a sequence into the tree
     */
    void insert(const vector<T>& sequence)
    {
        clTrieNode* cur = this;
        for(const auto& d : sequence) {
            if(cur->m_children.count(d) == 0) {
                cur->m_children.insert({ d, new clTrieNode() });
            }
            cur = cur->m_children[d].second;
        }

        // mark the last node as a "end of sequence"
        cur->end_of_sequence = true;
    }

    /**
     * @brief return if the tree contains a given sequence
     */
    bool contains(const vector<T>& sequence)
    {
        clTrieNode* cur = this;
        for(const auto& d : sequence) {
            if(cur->m_children.count(d) == 0) {
                return false;
            }
            cur = cur->m_children[d].second;
        }

        // if we reached here, it means that we were able to match the entire
        // sequence, however, we only return true if this entire sequence was
        // inserted and its not a sub sequence of something else
        return cur->end_of_sequence;
    }
};
#endif // TRIE_HPP
