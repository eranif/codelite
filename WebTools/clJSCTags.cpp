#include "clJSCTags.h"
#include "event_notifier.h"
#include "codelite_events.h"
#include <wx/filename.h>
#include "clZipReader.h"
#include "jobqueue.h"
#include "job.h"
#include "cl_standard_paths.h"

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
        m_jsctags->CallAfter(&clJSCTags::ZipExtractCompleted);
    }
};

clJSCTags::clJSCTags()
    : m_zipExtracted(false)
{
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &clJSCTags::OnEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &clJSCTags::OnEditorSaved, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, &clJSCTags::OnInitDone, this);
}

clJSCTags::~clJSCTags()
{
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &clJSCTags::OnEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &clJSCTags::OnEditorSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_INIT_DONE, &clJSCTags::OnInitDone, this);
}

void clJSCTags::OnEditorSaved(clCommandEvent& event) { event.Skip(); }

void clJSCTags::OnEditorChanged(wxCommandEvent& event) { event.Skip(); }

void clJSCTags::ZipExtractCompleted() { m_zipExtracted = true; }

void clJSCTags::OnInitDone(wxCommandEvent& event)
{
    // Extract the zip file
    wxFileName jsctagsZip(clStandardPaths::Get().GetDataDir(), "jsctags.zip");
    if(jsctagsZip.Exists()) {
        wxFileName targetDir(clStandardPaths::Get().GetUserDataDir(), "");
        targetDir.AppendDir("webtools");
        targetDir.AppendDir("jsctags");
        JobQueueSingleton::Instance()->PushJob(
            new clJSCTagsZipJob(this, jsctagsZip.GetFullPath(), targetDir.GetPath()));
    }
}
