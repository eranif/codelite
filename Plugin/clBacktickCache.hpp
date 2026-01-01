#ifndef CLBACKTICKCACHE_HPP
#define CLBACKTICKCACHE_HPP

#include "codelite_exports.h"
#include "macros.h"

#include <memory>
#include <wx/string.h>

class WXDLLIMPEXP_SDK clBacktickCache
{
    wxString m_file;
    wxStringMap_t m_cache;

public:
    using ptr_t = std::shared_ptr<clBacktickCache>;

public:
    clBacktickCache(const wxString& directory);
    virtual ~clBacktickCache() = default;

    void Save();

    bool HasCommand(const wxString& command) const;
    void SetCommand(const wxString& command, const wxString& expanded);
    wxString GetExpanded(const wxString& command) const;
};

#endif // CLBACKTICKCACHE_HPP
