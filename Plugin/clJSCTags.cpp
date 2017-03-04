#include "clJSCTags.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include <wx/filename.h>
#include "clZipReader.h"
#include "jobqueue.h"
#include "job.h"
#include "cl_standard_paths.h"
#include "globals.h"
#include "ieditor.h"
#include "macros.h"
#include <imanager.h>
#include "processreaderthread.h"
#include "file_logger.h"

class clJSCTagsZipJob : public Job
{
protected:
    clJSCTags* m_jsctags;
    wxString m_zipfile;
    wxString m_targetFolder;

public:
    clJSCTagsZipJob(clJSCTags* jsctags, const wxString& zipfile, const wxString& targetFolder)
        : Job(jsctags)
        , m_jsctags(jsctags)
        , m_zipfile(zipfile)
        , m_targetFolder(targetFolder)
    {
    }

    virtual ~clJSCTagsZipJob() {}

    void Process(wxThread* thread)
    {
        wxUnusedVar(thread);
        // Extract the zip file
        wxFileName::Mkdir(m_targetFolder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        clZipReader zipReader(m_zipfile);
        zipReader.Extract("*", m_targetFolder);
        m_jsctags->ZipExtractCompleted();
    }
};

clJSCTags::clJSCTags()
    : m_zipExtracted(false)
    , m_process(NULL)
{
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &clJSCTags::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &clJSCTags::OnEditorSaved, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &clJSCTags::OnInitDone, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CLOSING, &clJSCTags::OnEditorClosing, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clJSCTags::OnZipProcessTerminated, this);
}

clJSCTags::~clJSCTags()
{
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &clJSCTags::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &clJSCTags::OnEditorSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &clJSCTags::OnInitDone, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CLOSING, &clJSCTags::OnEditorClosing, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clJSCTags::OnZipProcessTerminated, this);
}

void clJSCTags::OnEditorSaved(clCommandEvent& event)
{
    event.Skip();
    if(!m_zipExtracted) return;

    wxString filename = event.GetFileName();
}

void clJSCTags::OnEditorChanged(wxCommandEvent& event)
{
    event.Skip();
    if(!m_zipExtracted) return;

    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);
}

void clJSCTags::ZipExtractCompleted() { m_zipExtracted = true; }

void clJSCTags::OnInitDone(wxCommandEvent& event)
{
    event.Skip();
    // Extract the zip file
    wxFileName jsctagsZip(clStandardPaths::Get().GetDataDir(), "jsctags.zip");
    if(!jsctagsZip.Exists()) return;
    wxFileName targetDir(clStandardPaths::Get().GetUserDataDir(), "");
    targetDir.AppendDir("webtools");
    targetDir.AppendDir("jsctags");
#ifdef __WXMSW__
    JobQueueSingleton::Instance()->PushJob(new clJSCTagsZipJob(this, jsctagsZip.GetFullPath(), targetDir.GetPath()));
#else
    // Create the target directory
    wxFileName::Mkdir(targetDir.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    // Build the unzip command
    wxString command;
    wxString zipfile = jsctagsZip.GetFullPath();
    ::WrapWithQuotes(zipfile);

    command << "/usr/bin/unzip " << zipfile;
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault, targetDir.GetPath());
    if(!m_process) {
        clERROR() << "Failed to execute process" << command << clEndl;
        return;
    }
#endif
}

void clJSCTags::OnEditorClosing(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = (IEditor*)e.GetClientData();
    CHECK_PTR_RET(editor);

    wxString closingpath = editor->GetFileName().GetFullPath();
    // Clear this file's cache
}

void clJSCTags::GTKExtractZip() {}

void clJSCTags::OnZipProcessTerminated(clProcessEvent& event)
{
    clDEBUG() << "unzipping of jsctags completed successfully" << clEndl;
    ZipExtractCompleted();
    wxDELETE(m_process);
}
