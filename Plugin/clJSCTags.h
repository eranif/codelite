#ifndef CLJSCTAGS_H
#define CLJSCTAGS_H

#include "cl_command_event.h"
#include "codelite_exports.h"
#include "smart_ptr.h"

#include <wx/event.h>

class WXDLLIMPEXP_SDK clJSCTags : public wxEvtHandler
{
    bool m_zipExtracted;

public:
    typedef SmartPtr<clJSCTags> Ptr_t;

protected:
public:
    clJSCTags();
    virtual ~clJSCTags();

    void ZipExtractCompleted();
    void OnEditorSaved(clCommandEvent& event);
    void OnEditorChanged(wxCommandEvent& event);
    void OnInitDone(wxCommandEvent& event);
    void OnEditorClosing(wxCommandEvent& e);
};

#endif // CLJSCTAGS_H
