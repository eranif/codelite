#ifndef CLFILECACHE_HPP
#define CLFILECACHE_HPP

#include <codelite_exports.h>
#include <vector>
#include <unordered_set>
#include <wx/filename.h>
#include <wxStringHash.h>

class WXDLLIMPEXP_SDK clFileCache
{
    std::vector<wxFileName> m_files;
    std::unordered_set<wxString> m_filesSet;

public:
    typedef std::vector<wxFileName>::const_iterator const_iterator;
    typedef std::vector<wxFileName>::iterator iterator;

public:
    clFileCache() {}
    ~clFileCache() {}

    const std::vector<wxFileName>& GetFiles() const { return m_files; }
    const_iterator begin() const { return m_files.begin(); }
    const_iterator end() const { return m_files.end(); }

    void Alloc(size_t size);
    void Add(const wxFileName& fn);
    void Clear();
    bool Contains(const wxFileName& fn) const;
    size_t GetSize() const { return m_files.size(); }
    bool IsEmpty() const { return m_files.empty(); }
};

#endif // CLFILECACHE_HPP
