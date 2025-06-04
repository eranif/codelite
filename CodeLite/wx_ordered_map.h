//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : wx_ordered_map.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef WX_ORDERED_MAP_H
#define WX_ORDERED_MAP_H

#include "codelite_exports.h"

#include <list>
#include <map>

/// A container that allows fast insertions / retrieval of elements
/// And also maintains the order of which elements where inserted
template <typename Key, typename Value>
class wxOrderedMap final
{
    using Pair_t = std::pair<const Key, Value>;
    using List_t = std::list<Pair_t>;
    using Map_t = std::map<Key, typename List_t::iterator>;

public:
    using ConstIterator = typename List_t::const_iterator;
    using const_iterator = typename List_t::const_iterator;
    using Iterator = typename List_t::iterator;
    using iterator = typename List_t::iterator;

private:
    Map_t m_map;
    List_t m_list;

public:
    wxOrderedMap() = default;
    ~wxOrderedMap() = default;

    wxOrderedMap(const wxOrderedMap&) = delete;
    wxOrderedMap(wxOrderedMap&&) = default;

    wxOrderedMap& operator=(const wxOrderedMap&) = delete;
    wxOrderedMap& operator=(wxOrderedMap&&) = default;

    /**
     * @brief fast swap implementation
     */
    void Swap(wxOrderedMap& other)
    {
        m_map.swap(other.m_map);
        m_list.swap(other.m_list);
    }

    void Clear()
    {
        m_map.clear();
        m_list.clear();
    }

    void PushBack(const Key& k, const Value& v)
    {
        Remove(k);
        Iterator iter = m_list.emplace(m_list.end(), k, v);
        m_map.emplace(k, iter);
    }

    void PushBack(const Key& k, Value&& v)
    {
        Remove(k);
        Iterator iter = m_list.emplace(m_list.end(), k, std::move(v));
        m_map.emplace(k, iter);
    }

    void PushFront(const Key& k, const Value& v)
    {
        Remove(k);
        Iterator iter = m_list.emplace(m_list.begin(), k, v);
        m_map.emplace(k, iter);
    }

    void PushFront(const Key& k, Value&& v)
    {
        Remove(k);
        Iterator iter = m_list.emplace(m_list.begin(), k, std::move(v));
        m_map.emplace(k, iter);
    }

    Value& Item(const Key& k)
    {
        static Value NullValue{};
        auto iter = m_map.find(k);
        if (iter == m_map.end()) {
            return NullValue;
        } else {
            return iter->second->second;
        }
    }

    const Value& Item(const Key& k) const
    {
        static const Value NullValue{};
        auto iter = m_map.find(k);
        if (iter == m_map.end()) {
            return NullValue;
        } else {
            return iter->second->second;
        }
    }

    bool Contains(const Key& k) const { return m_map.count(k); }

    void Remove(const Key& k)
    {
        auto iter = m_map.find(k);
        if (iter == m_map.end()) {
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
    ConstIterator Begin() const { return m_list.begin(); }
    Iterator Begin() { return m_list.begin(); }

    const_iterator begin() const { return m_list.begin(); }
    iterator begin() { return m_list.begin(); }
};

#endif // WX_ORDERED_MAP_H
