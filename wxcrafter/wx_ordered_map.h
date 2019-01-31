#ifndef WX_ORDERED_MAP_H
#define WX_ORDERED_MAP_H

#include <list>
#include <map>

/// A container that allows fast insertions / retrieval of elements
/// And also maintains the order of which elements where inserted
template <typename Key, typename Value> class wxOrderedMap
{
    typedef std::pair<Key, Value> Pair_t;
    typedef std::list<Pair_t> List_t;
    typedef std::map<Key, typename List_t::iterator> Map_t;

public:
    typedef typename List_t::const_iterator ConstIterator;
    typedef typename List_t::const_iterator const_iterator;
    typedef typename List_t::iterator Iterator;
    typedef typename List_t::iterator iterator;

protected:
    Map_t m_map;
    List_t m_list;

public:
    wxOrderedMap() {}
    virtual ~wxOrderedMap() {}

    wxOrderedMap(const wxOrderedMap& rhs)
    {
        if(this == &rhs) return;
        *this = rhs;
    }

    wxOrderedMap<Key, Value>& operator=(const wxOrderedMap& rhs)
    {
        if(this == &rhs) return *this;

        Clear();
        m_map.insert(rhs.m_map.begin(), rhs.m_map.end());
        m_list.insert(m_list.end(), rhs.m_list.begin(), rhs.m_list.end());
        return *this;
    }

    void Clear()
    {
        m_map.clear();
        m_list.clear();
    }

    void PushBack(const Key& k, const Value& v)
    {
        if(Contains(k)) {
            // we already got an instance of this item
            Remove(k);
        }
        Iterator iter = m_list.insert(m_list.end(), Pair_t(k, v));
        m_map.insert(std::make_pair(k, iter));
    }

    void PushFront(const Key& k, const Value& v)
    {
        if(Contains(k)) {
            // we already got an instance of this item
            Remove(k);
        }
        Iterator iter = m_list.insert(m_list.begin(), Pair_t(k, v));
        m_map.insert(std::make_pair(k, iter));
    }

    Value& Item(const Key& k)
    {
        static Value NullValue;
        typename Map_t::iterator iter = m_map.find(k);
        if(iter == m_map.end())
            return NullValue;
        else {
            return iter->second->second;
        }
    }

    const Value& Item(const Key& k) const
    {
        static Value NullValue;
        typename Map_t::const_iterator iter = m_map.find(k);
        if(iter == m_map.end())
            return NullValue;
        else {
            return iter->second->second;
        }
    }

    bool Contains(const Key& k) const { return m_map.count(k); }

    void Remove(const Key& k)
    {
        typename Map_t::iterator iter = m_map.find(k);
        if(iter == m_map.end()) {
            // nothing to remove here
            return;
        }

        m_list.erase(iter->second);
        m_map.erase(iter);
    }

    /**
     * @brief is empty?
     */
    bool IsEmpty() const { return m_list.empty(); }

    // end()
    ConstIterator End() const { return m_list.end(); }

    Iterator End() { return m_list.end(); }

    const_iterator end() const { return m_list.end(); }

    iterator end() { return m_list.end(); }

    // begin()
    Iterator Begin() { return m_list.begin(); }

    ConstIterator Begin() const { return m_list.begin(); }

    const_iterator begin() const { return m_list.begin(); }

    iterator begin() { return m_list.begin(); }

    void DeleteValues()
    {
        typename List_t::iterator iter = m_list.begin();
        for(; iter != m_list.end(); ++iter) {
            Value v = (*iter).second;
            delete v;
        }
        Clear();
    }
};

#endif // WX_ORDERED_MAP_H
