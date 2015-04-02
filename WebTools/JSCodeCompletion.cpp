#include "JSCodeCompletion.h"
#include <wx/stc/stc.h>
#include "macros.h"
#include <wx/xrc/xmlres.h>
#include <wx/app.h>
#include "wxCodeCompletionBoxEntry.h"
#include <algorithm>
#include "wxCodeCompletionBoxManager.h"
#include "cl_standard_paths.h"
#include "clZipReader.h"
#include <wx/filename.h>
#include "entry.h"
#include "cl_calltip.h"
#include <wx/log.h>
#include <wx/filename.h>
#include "json_node.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"

#ifdef __WXMSW__
#define ZIP_NAME "javascript-win.zip"
#elif defined(__WXGTK__)
#define ZIP_NAME "javascript.zip"
#else
#define ZIP_NAME "javascript-osx.zip"
#endif


JSCodeCompletion::JSCodeCompletion()
    : m_thread(NULL)
    , m_ternServer(this)
    , m_ccPos(wxNOT_FOUND)
{
    m_thread = new JSParserThread(this);
    m_thread->Start();

    wxFileName jsResources(clStandardPaths::Get().GetDataDir(), ZIP_NAME);
    if(jsResources.Exists()) {

        clZipReader zipReader(jsResources);
        wxFileName targetDir(clStandardPaths::Get().GetUserDataDir(), "");
        targetDir.AppendDir("webtools");
        targetDir.AppendDir("js");

        targetDir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        zipReader.Extract("*", targetDir.GetPath());

        m_thread = new JSParserThread(this);
        m_thread->Start();
        
        m_ternServer.Start();
    }
}

JSCodeCompletion::~JSCodeCompletion()
{
    m_ternServer.Terminate();
    if(m_thread) {
        m_thread->Stop();
        wxDELETE(m_thread);
    }
}

void JSCodeCompletion::CodeComplete(IEditor* editor)
{
    CHECK_PTR_RET(editor);
    wxStyledTextCtrl* ctrl = editor->GetSTC();

    // work until the current buffer
    wxString buffer = ctrl->GetTextRange(0, ctrl->GetCurrentPos());
    CHECK_COND_RET(!buffer.IsEmpty());
    
    wxFileName tmpFileName = wxFileName::CreateTempFileName("tern");
    if(!FileUtils::WriteFileContent(tmpFileName, ctrl->GetText())) return;
    
    // Request coompletion entries
    {
        JSONRoot root(cJSON_Object);
        JSONElement query = JSONElement::createObject("query");
        root.toElement().append(query);
        query.addProperty("type", wxString("completions"));
        query.addProperty("file", tmpFileName.GetFullPath());
        query.addProperty("end", ctrl->GetCurrentPos());
        query.addProperty("docs", true);
        query.addProperty("urls", true);
        query.addProperty("includeKeywords", true);
        query.addProperty("types", true);
        m_ternServer.PostRequest(root.toElement().format(), editor->GetFileName(), tmpFileName);
        m_ccPos = ctrl->GetCurrentPos();
    }
}

void JSCodeCompletion::PraserThreadCompleted(JSParserThread::Reply* reply) { wxDELETE(reply); }

void JSCodeCompletion::OnCodeCompleteReady(const wxCodeCompletionBoxEntry::Vec_t& entries, const wxString& filename)
{
    IEditor* editor = ::clGetManager()->GetActiveEditor();
    
    // sanity
    CHECK_PTR_RET(editor);
    if(editor->GetFileName().GetFullPath() != filename) return;
    if(editor->GetCurrentPosition() != m_ccPos) return;
    if(entries.empty()) return;
    
    wxStyledTextCtrl* ctrl = editor->GetSTC();
    wxCodeCompletionBoxManager::Get().ShowCompletionBox(ctrl, entries, 0, this);
}
