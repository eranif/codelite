#ifndef CLJSCTAGS_H
#define CLJSCTAGS_H

#include <wx/event.h>
#include "cl_command_event.h"
#include "smart_ptr.h"

class clJSCTags : public wxEvtHandler
{
    bool m_zipExtracted;

public:
    typedef SmartPtr<clJSCTags> Ptr_t;

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
