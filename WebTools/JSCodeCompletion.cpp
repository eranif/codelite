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
}

JSCodeCompletion::~JSCodeCompletion() { m_ternServer.Terminate(); }

void JSCodeCompletion::CodeComplete(IEditor* editor)
{
    if(!IsEnabled()) {
        TriggerWordCompletion();
        return;
    }

#ifdef __WXGTK__
    wxFileName nodeJS;
    if(!clTernServer::LocateNodeJS(nodeJS)) {
        wxString msg;
        msg << _("It seems that NodeJS is not installed on your machine\n(Can't find file "
                 "'/usr/bin/nodejs' or '/usr/bin/node')\nI have temporarily disabled Code Completion for "
                 "JavaScript\nPlease install "
                 "NodeJS and try again");
        wxMessageBox(msg, "CodeLite", wxICON_WARNING | wxOK | wxCENTER);

        // Disable CC
        WebToolsConfig conf;
        conf.Load().EnableJavaScriptFlag(WebToolsConfig::kJSEnableCC, false);
        conf.Save();
        return;
    }
#endif

    // Sanity
    CHECK_PTR_RET(editor);

    // Check the completion type
    wxStyledTextCtrl* ctrl = editor->GetCtrl();
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

    m_ccPos = ctrl->GetCurrentPos();
    if(!isFunctionTip) {
        m_ternServer.PostCCRequest(editor);

    } else {
        m_ternServer.PostFunctionTipRequest(editor, currentPos);
    }
}

void JSCodeCompletion::OnCodeCompleteReady(const wxCodeCompletionBoxEntry::Vec_t& entries, const wxString& filename)
{
    IEditor* editor = ::clGetManager()->GetActiveEditor();

    // sanity
    CHECK_PTR_RET(editor);
    if(editor->GetFileName().GetFullPath() != filename) return;
    if(editor->GetCurrentPosition() != m_ccPos) return;
    if(entries.empty()) {
        TriggerWordCompletion();
        return;
    }

    wxStyledTextCtrl* ctrl = editor->GetCtrl();
    wxCodeCompletionBoxManager::Get().ShowCompletionBox(ctrl, entries, 0, wxNOT_FOUND, this);
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

void JSCodeCompletion::Reload() { m_ternServer.RecycleIfNeeded(true); }

bool JSCodeCompletion::IsEnabled() const
{
    WebToolsConfig conf;
    return conf.Load().HasJavaScriptFlag(WebToolsConfig::kJSEnableCC);
}

void JSCodeCompletion::TriggerWordCompletion()
{
    // trigger word completion
    wxCommandEvent wordCompleteEvent(wxEVT_MENU, XRCID("word_complete_no_single_insert"));
    wxTheApp->ProcessEvent(wordCompleteEvent);
}
