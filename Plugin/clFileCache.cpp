#include "clFileCache.hpp"

void clFileCache::Add(const wxFileName& fn)
{
    if(Contains(fn)) {
        return;
    }
    m_files.push_back(fn);
    m_filesSet.insert(fn.GetFullPath());
}

void clFileCache::Clear()
{
    m_filesSet.clear();
    m_files.clear();
}

bool clFileCache::Contains(const wxFileName& fn) const { return m_filesSet.count(fn.GetFullPath()); }

void clFileCache::Alloc(size_t size)
{
    m_files.reserve(size);
    m_filesSet.reserve(size);
}
