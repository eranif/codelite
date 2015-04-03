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
#include "WebToolsConfig.h"

#ifdef __WXMSW__
#define ZIP_NAME "javascript-win.zip"
#elif defined(__WXGTK__)
#define ZIP_NAME "javascript.zip"
#else
#define ZIP_NAME "javascript-osx.zip"
#endif

JSCodeCompletion::JSCodeCompletion()
    : m_ternServer(this)
    , m_ccPos(wxNOT_FOUND)
{
    wxFileName jsResources(clStandardPaths::Get().GetDataDir(), ZIP_NAME);
    if(jsResources.Exists()) {

        clZipReader zipReader(jsResources);
        wxFileName targetDir(clStandardPaths::Get().GetUserDataDir(), "");
        targetDir.AppendDir("webtools");
        targetDir.AppendDir("js");

        targetDir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        zipReader.Extract("*", targetDir.GetPath());

        m_ternServer.Start();
    }

    WebToolsConfig conf;
    m_enabled = conf.Load().HasJavaScriptFlag(WebToolsConfig::kJSEnableCC);
}

JSCodeCompletion::~JSCodeCompletion() { m_ternServer.Terminate(); }

void JSCodeCompletion::CodeComplete(IEditor* editor)
{
    if(!m_enabled) return;

    // Check that NodeJS is installed
#ifdef __WXGTK__
    wxFileName nodeJS("/usr/bin", "nodejs");
    if(!nodeJS.FileExists()) {
        wxString msg;
        msg << _("It seems that NodeJS is not installed on your machine\n(Can't find file "
                  "'/usr/bin/nodejs')\nI have temporarily disabled Code Completion for JavaScript\nPlease install NodeJS and try again");
        wxMessageBox(msg, "CodeLite", wxICON_WARNING|wxOK|wxCENTER);
        
        // Disable CC
        WebToolsConfig conf;
        conf.Load().EnableJavaScriptFlag(WebToolsConfig::kJSEnableCC, false);
        conf.Save();
        return;
    }
#endif

    CHECK_PTR_RET(editor);
    wxStyledTextCtrl* ctrl = editor->GetSTC();

    // work until the current buffer
    wxString buffer = ctrl->GetTextRange(0, ctrl->GetCurrentPos());
    CHECK_COND_RET(!buffer.IsEmpty());

    wxFileName tmpFileName = wxFileName::CreateTempFileName("tern");
    if(!FileUtils::WriteFileContent(tmpFileName, ctrl->GetText())) return;

    // Request coompletion entries

    // Check the completion type
    int currentPos = ctrl->PositionBefore(ctrl->GetCurrentPos());
    bool isFunctionTip = false;
    while(currentPos > 0) {
        char prevChar = ctrl->GetCharAt(currentPos);
        if((prevChar == ' ') || (prevChar == '\t') || (prevChar == '\n') || (prevChar == '\r')) {
            currentPos = ctrl->PositionBefore(currentPos);
            continue;
        }
        if(prevChar == '(') {
            isFunctionTip = true;
        }
        break;
    }

    if(!isFunctionTip) {
        JSONRoot root(cJSON_Object);
        JSONElement query = JSONElement::createObject("query");
        root.toElement().append(query);
        query.addProperty("type", wxString("completions"));
        query.addProperty("file", tmpFileName.GetFullPath());

        // Use {line, col} object
        JSONElement loc = JSONElement::createObject("end");
        query.append(loc);
        loc.addProperty("line", ctrl->GetCurrentLine());
        loc.addProperty("ch", ctrl->GetColumn(ctrl->GetCurrentPos()));

        query.addProperty("expandWordForward", false);
        query.addProperty("docs", true);
        query.addProperty("urls", true);
        query.addProperty("includeKeywords", true);
        query.addProperty("types", true);
        m_ternServer.PostRequest(root.toElement().format(), editor->GetFileName(), tmpFileName, isFunctionTip);
        m_ccPos = ctrl->GetCurrentPos();

    } else {
        JSONRoot root(cJSON_Object);
        JSONElement query = JSONElement::createObject("query");
        root.toElement().append(query);
        query.addProperty("type", wxString("type"));
        query.addProperty("file", tmpFileName.GetFullPath());

        // Use {line, col} object
        JSONElement loc = JSONElement::createObject("end");
        query.append(loc);
        loc.addProperty("line", ctrl->LineFromPosition(currentPos));
        loc.addProperty("ch", ctrl->GetColumn(currentPos));

        m_ternServer.PostRequest(root.toElement().format(), editor->GetFileName(), tmpFileName, isFunctionTip);
        m_ccPos = ctrl->GetCurrentPos();
    }
}

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

void JSCodeCompletion::OnFunctionTipReady(clCallTipPtr calltip, const wxString& filename)
{
    IEditor* editor = ::clGetManager()->GetActiveEditor();

    // sanity
    CHECK_PTR_RET(editor);
    CHECK_PTR_RET(calltip);
    if(editor->GetFileName().GetFullPath() != filename) return;
    if(editor->GetCurrentPosition() != m_ccPos) return;
    editor->ShowCalltip(calltip);
}

void JSCodeCompletion::Reload()
{
    WebToolsConfig conf;
    m_enabled = conf.Load().HasJavaScriptFlag(WebToolsConfig::kJSEnableCC);

    m_ternServer.RecycleIfNeeded(true);
}
