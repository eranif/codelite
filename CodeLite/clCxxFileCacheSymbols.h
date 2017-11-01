#ifndef CLCXXFILECACHESYMBOLS_H
#define CLCXXFILECACHESYMBOLS_H

#include "codelite_exports.h"
#include "wxStringHash.h"
#include "entry.h"
#include <vector>
#include <wx/filename.h>
#include <wx/thread.h>
#include <wx/event.h>
#include "cl_command_event.h"
#include <wx/sharedptr.h>

class WXDLLIMPEXP_CL clCxxFileCacheSymbols : public wxEvtHandler
{
    std::unordered_map<wxString, std::vector<TagEntryPtr> > m_cache;
    wxCriticalSection m_cs;

protected:
    void OnFileSave(clCommandEvent& e);
    void OnWorkspaceAction(wxCommandEvent& e);
    void OnPraseCompleted(clCommandEvent& e);

public:
    enum eFilter {
        kAllSymbols = 0,
        kFunctions = (1 << 0),
    };

public:
    typedef wxSharedPtr<clCxxFileCacheSymbols> Ptr_t;
    clCxxFileCacheSymbols();
    virtual ~clCxxFileCacheSymbols();

    bool Contains(const wxFileName& filename);
    bool Contains(const wxString& filename);
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
#endif // CLCXXFILECACHESYMBOLS_H
