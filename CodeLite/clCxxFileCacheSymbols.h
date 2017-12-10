#ifndef CLCXXFILECACHESYMBOLS_H
#define CLCXXFILECACHESYMBOLS_H

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "entry.h"
#include "wxStringHash.h"
#include <vector>
#include <wx/event.h>
#include <wx/filename.h>
#include <wx/sharedptr.h>
#include <wx/thread.h>

class SourceToTagsThread;
class WXDLLIMPEXP_CL clCxxFileCacheSymbols : public wxEvtHandler
{
    std::unordered_map<wxString, std::vector<TagEntryPtr> > m_cache;
    std::unordered_set<wxString> m_pendingFiles;
    wxCriticalSection m_cs;
    SourceToTagsThread* m_helperThread;

protected:
    void OnFileSave(clCommandEvent& e);
    void OnWorkspaceAction(wxCommandEvent& e);

public:
    void OnPraseCompleted(const wxString& filename, const wxString& strTags);

public:
    enum eFilter {
        kAllSymbols = 0,
        kFunctions = (1 << 0),
    };

public:
    typedef wxSharedPtr<clCxxFileCacheSymbols> Ptr_t;
    clCxxFileCacheSymbols();
    virtual ~clCxxFileCacheSymbols();

    bool Contains(const wxFileName& filename) const;
    bool Contains(const wxString& filename) const;
    void Clear();
    void Update(const wxFileName& filename, const TagEntryPtrVector_t& tags);
    void Delete(const wxFileName& filename);

    /**
     * @brief fetch from the cache, apply "flags" on the result. See eFilter
     */
    bool Find(const wxFileName& filename, TagEntryPtrVector_t& tags, size_t flags = 0);

    /**
     * @brief Request parsing of a file from the parser thread. The results will be cached
     * and an event 'clCommandEvent' is fired to notify that the cache was updated
     */
    void RequestSymbols(const wxFileName& filename);
};

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CXX_SYMBOLS_CACHE_UPDATED, clCommandEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxEVT_CXX_SYMBOLS_CACHE_INVALIDATED, clCommandEvent);
#endif // CLCXXFILECACHESYMBOLS_H
