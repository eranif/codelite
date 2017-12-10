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
#include "file_logger.h"

class clJSCTagsZipJob : public Job
{
protected:
    clJSCTags* m_jsctags;
    wxString m_zipfile;
    wxString m_targetFolder;

public:
    clJSCTagsZipJob(const wxString& zipfile, const wxString& targetFolder)
        : Job()
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
        clDEBUG() << "Extracting zip file:" << m_zipfile << clEndl;
        zipReader.Extract("*", m_targetFolder);
        clDEBUG() << "Extracting zip file:" << m_zipfile << "...done" << clEndl;
    }
};

clJSCTags::clJSCTags()
    : m_zipExtracted(false)
{
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &clJSCTags::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &clJSCTags::OnEditorSaved, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &clJSCTags::OnInitDone, this);
    EventNotifier::Get()->Bind(wxEVT_EDITOR_CLOSING, &clJSCTags::OnEditorClosing, this);
}

clJSCTags::~clJSCTags()
{
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &clJSCTags::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &clJSCTags::OnEditorSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &clJSCTags::OnInitDone, this);
    EventNotifier::Get()->Unbind(wxEVT_EDITOR_CLOSING, &clJSCTags::OnEditorClosing, this);
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
#if 0
#ifndef __WXGTK__
    JobQueueSingleton::Instance()->PushJob(new clJSCTagsZipJob(jsctagsZip.GetFullPath(), targetDir.GetPath()));
#else
    clJSCTagsZipJob job(jsctagsZip.GetFullPath(), targetDir.GetPath());
    job.Process(NULL);
#endif
#endif
    ZipExtractCompleted();
}

void clJSCTags::OnEditorClosing(wxCommandEvent& e)
{
    e.Skip();
    IEditor* editor = (IEditor*)e.GetClientData();
    CHECK_PTR_RET(editor);

    wxString closingpath = editor->GetFileName().GetFullPath();
    // Clear this file's cache
}
