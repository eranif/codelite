#ifndef CLBACKTICKCACHE_HPP
#define CLBACKTICKCACHE_HPP

#include "codelite_exports.h"
#include "macros.h"
#include "wxStringHash.h"

#include <wx/sharedptr.h>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clBacktickCache
{
    wxString m_file;
    wxStringMap_t m_cache;

public:
    typedef wxSharedPtr<clBacktickCache> ptr_t;

public:
    clBacktickCache(const wxString& directory);
    virtual ~clBacktickCache();

    void Save();

    bool HasCommand(const wxString& command) const;
    void SetCommand(const wxString& command, const wxString& expanded);
    wxString GetExpanded(const wxString& command) const;
};

#endif // CLBACKTICKCACHE_HPP
