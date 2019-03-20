#include "JSCodeCompletion.h"
#include "WebToolsConfig.h"
#include "clZipReader.h"
#include "cl_calltip.h"
#include "cl_standard_paths.h"
#include "entry.h"
#include "event_notifier.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"
#include "JSON.h"
#include "macros.h"
#include "navigationmanager.h"
#include "wxCodeCompletionBoxEntry.h"
#include "wxCodeCompletionBoxManager.h"
#include <algorithm>
#include <wx/app.h>
#include <wx/filename.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/stc/stc.h>
#include <wx/xrc/xmlres.h>
#include "clNodeJS.h"
#include "webtools.h"
#include "codelite_events.h"

JSCodeCompletion::JSCodeCompletion(const wxString& workingDirectory, WebTools* plugin)
    : ServiceProvider("WebTools: JavaScript", eServiceType::kCodeCompletion)
    , m_ternServer(this)
    , m_ccPos(wxNOT_FOUND)
    , m_workingDirectory(workingDirectory)
    , m_plugin(plugin)
{
    wxTheApp->Bind(wxEVT_MENU, &JSCodeCompletion::OnGotoDefinition, this, XRCID("ID_MENU_JS_GOTO_DEFINITION"));
    if(WebToolsConfig::Get().IsTernInstalled() && WebToolsConfig::Get().IsNodeInstalled()) {
        m_ternServer.Start(m_workingDirectory);
    }
    EventNotifier::Get()->Bind(wxEVT_INFO_BAR_BUTTON, &JSCodeCompletion::OnInfoBarClicked, this);
    Bind(wxEVT_CC_FIND_SYMBOL, &JSCodeCompletion::OnFindSymbol, this);
    Bind(wxEVT_CC_CODE_COMPLETE, &JSCodeCompletion::OnCodeComplete, this);
    Bind(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &JSCodeCompletion::OnCodeCompleteFunctionCalltip, this);
}

JSCodeCompletion::~JSCodeCompletion()
{
    m_ternServer.Terminate();
    wxTheApp->Unbind(wxEVT_MENU, &JSCodeCompletion::OnGotoDefinition, this, XRCID("ID_MENU_JS_GOTO_DEFINITION"));
    EventNotifier::Get()->Unbind(wxEVT_INFO_BAR_BUTTON, &JSCodeCompletion::OnInfoBarClicked, this);
    Unbind(wxEVT_CC_FIND_SYMBOL, &JSCodeCompletion::OnFindSymbol, this);
    Unbind(wxEVT_CC_CODE_COMPLETE, &JSCodeCompletion::OnCodeComplete, this);
    Unbind(wxEVT_CC_CODE_COMPLETE_FUNCTION_CALLTIP, &JSCodeCompletion::OnCodeCompleteFunctionCalltip, this);
}

bool JSCodeCompletion::SanityCheck()
{
    WebToolsConfig& conf = WebToolsConfig::Get();
    if(!conf.IsNodeInstalled() || !conf.IsNpmInstalled()) {
        CallAfter(&JSCodeCompletion::DoPromptForInstallNodeJS);

        // Disable CC
        conf.EnableJavaScriptFlag(WebToolsConfig::kJSEnableCC, false);
        return false;
    }

    // Locate tern
    if(!conf.IsTernInstalled()) {
        CallAfter(&JSCodeCompletion::DoPromptForInstallTern);
        // Disable CC
        conf.EnableJavaScriptFlag(WebToolsConfig::kJSEnableCC, false);
        return false;
    }
    return true;
}

void JSCodeCompletion::CodeComplete(IEditor* editor)
{
    if(!IsEnabled()) {
        TriggerWordCompletion();
        return;
    }

    if(!SanityCheck()) return;

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
        if(prevChar == '(') { isFunctionTip = true; }
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
    WebToolsConfig& conf = WebToolsConfig::Get();
    return conf.HasJavaScriptFlag(WebToolsConfig::kJSEnableCC);
}

void JSCodeCompletion::TriggerWordCompletion()
{
    // trigger word completion
    wxCommandEvent wordCompleteEvent(wxEVT_MENU, XRCID("simple_word_completion"));
    EventNotifier::Get()->TopFrame()->GetEventHandler()->AddPendingEvent(wordCompleteEvent);
}

void JSCodeCompletion::FindDefinition(IEditor* editor)
{
    if(!IsEnabled()) { return; }

    if(!SanityCheck()) return;

    // Sanity
    CHECK_PTR_RET(editor);

    wxStyledTextCtrl* ctrl = editor->GetCtrl();
    m_ccPos = ctrl->GetCurrentPos();
    m_ternServer.PostFindDefinitionRequest(editor);
}

void JSCodeCompletion::OnDefinitionFound(const clTernDefinition& loc)
{
    if(loc.IsURL()) {
        ::wxLaunchDefaultBrowser(loc.url);
    } else {
        BrowseRecord from, to;
        wxString pattern;
        if(clGetManager()->GetActiveEditor()) {
            pattern = clGetManager()->GetActiveEditor()->GetWordAtCaret();
            from = clGetManager()->GetActiveEditor()->CreateBrowseRecord();
        }
        IEditor* editor = clGetManager()->OpenFile(loc.file);
        if(editor) {
            editor->CenterLine(editor->LineFromPos(loc.start));
            if(editor->FindAndSelect(pattern, pattern, loc.start, NULL)) {
                to = editor->CreateBrowseRecord();
                // Record this jump
                clGetManager()->GetNavigationMgr()->AddJump(from, to);
            }
        }
    }
}

void JSCodeCompletion::ResetTern(bool force)
{
    if(!IsEnabled()) { return; }

    if(!SanityCheck()) return;

    // Sanity
    m_ccPos = wxNOT_FOUND;

    // recycle tern
    // m_ternServer.PostResetCommand(true);
    m_ternServer.RecycleIfNeeded(force);
}

void JSCodeCompletion::AddContextMenu(wxMenu* menu, IEditor* editor)
{
    wxUnusedVar(editor);
    menu->PrependSeparator();
    menu->Prepend(XRCID("ID_MENU_JS_GOTO_DEFINITION"), _("Find Definition"));
}

void JSCodeCompletion::OnGotoDefinition(wxCommandEvent& event)
{
    wxUnusedVar(event);
    FindDefinition(clGetManager()->GetActiveEditor());
}

void JSCodeCompletion::ReparseFile(IEditor* editor)
{
    if(!IsEnabled()) { return; }
    CHECK_PTR_RET(editor);

    if(!SanityCheck()) return;

    // Sanity
    m_ccPos = wxNOT_FOUND;
    m_ternServer.PostReparseCommand(editor);
}

void JSCodeCompletion::DoPromptForInstallNodeJS()
{
    wxString msg;
    msg << _("NodeJS and/or Npm are not installed on your machine. JavaScript code completion is disabled");
    clGetManager()->DisplayMessage(msg);
}

void JSCodeCompletion::DoPromptForInstallTern()
{
    // Show the info message
    clGetManager()->DisplayMessage(
        _("CodeLite uses 'tern' for JavaScript code completion. Would you like to install tern now?"), wxICON_QUESTION,
        { { XRCID("npm-install-tern"), _("Yes") }, { wxID_NO, "" } });
}

void JSCodeCompletion::OnInfoBarClicked(clCommandEvent& event)
{
    event.Skip(false);
    WebToolsConfig& conf = WebToolsConfig::Get();
    if(event.GetInt() == XRCID("npm-install-tern")) {
        clGetManager()->SetStatusMessage("npm install tern...", 5);
        clNodeJS::Get().NpmSilentInstall("tern", conf.GetTempFolder(true), "", m_plugin, "npm-install-tern");

    } else {
        event.Skip();
    }
}

void JSCodeCompletion::OnFindSymbol(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = dynamic_cast<IEditor*>(event.GetEditor());
    if(editor && m_plugin->IsJavaScriptFile(editor) && !m_plugin->InsideJSComment(editor)) {
        event.Skip(false);
        FindDefinition(editor);
    }
}

void JSCodeCompletion::OnCodeComplete(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = dynamic_cast<IEditor*>(event.GetEditor());
    if(editor && m_plugin->IsJavaScriptFile(editor)) {
        event.Skip(false);
        if(m_plugin->InsideJSComment(editor) || m_plugin->InsideJSString(editor)) {
            // User the word completion plugin instead
            TriggerWordCompletion();
        } else {
            CodeComplete(editor);
        }
    }
}

void JSCodeCompletion::OnCodeCompleteFunctionCalltip(clCodeCompletionEvent& event)
{
    event.Skip();
    IEditor* editor = dynamic_cast<IEditor*>(event.GetEditor());
    if(editor && m_plugin->IsJavaScriptFile(editor) && !m_plugin->InsideJSComment(editor)) {
        event.Skip(false);
        CodeComplete(editor);
    }
}
