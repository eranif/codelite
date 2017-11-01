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

public:
    typedef wxSharedPtr<clCxxFileCacheSymbols> Ptr_t;
    clCxxFileCacheSymbols();
    virtual ~clCxxFileCacheSymbols();

    bool Contains(const wxFileName& filename);
    bool Contains(const wxString& filename);
    void Clear();
    void Update(const wxFileName& filename, const TagEntryPtrVector_t& tags);
    void Delete(const wxFileName& filename);
    bool Find(const wxFileName& filename, TagEntryPtrVector_t& tags);
};

#endif // CLCXXFILECACHESYMBOLS_H
