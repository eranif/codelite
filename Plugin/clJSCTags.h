#ifndef CLJSCTAGS_H
#define CLJSCTAGS_H

#include <wx/event.h>
#include "cl_command_event.h"
#include "smart_ptr.h"
#include "codelite_exports.h"
#include "asyncprocess.h"

class WXDLLIMPEXP_SDK clJSCTags : public wxEvtHandler
{
    bool m_zipExtracted;
    IProcess* m_process;

public:
    typedef SmartPtr<clJSCTags> Ptr_t;

protected:
    void GTKExtractZip();
    void OnZipProcessTerminated(clProcessEvent& event);
    void OnZipProcessOutput(clProcessEvent& event);
    
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
