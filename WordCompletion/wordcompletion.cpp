#include "wordcompletion.h"
#include <wx/xrc/xmlres.h>
#include "WordCompletionThread.h"
#include <wx/stc/stc.h>
#include "clKeyboardManager.h"
#include <wx/app.h>
#include "WordCompletionSettingsDlg.h"
#include "event_notifier.h"
#include "cl_command_event.h"

static WordCompletionPlugin* thePlugin = NULL;

// Define the plugin entry point
extern "C" EXPORT IPlugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == 0) {
        thePlugin = new WordCompletionPlugin(manager);
    }
    return thePlugin;
}

extern "C" EXPORT PluginInfo GetPluginInfo()
{
    PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("Word Completion"));
    info.SetDescription(wxT("Suggest completion based on words typed in the editor"));
    info.SetVersion(wxT("v1.0"));
    return info;
}

extern "C" EXPORT int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

WordCompletionPlugin::WordCompletionPlugin(IManager* manager)
    : IPlugin(manager)
    , m_thread(NULL)
{
    m_longName = wxT("Suggest completion based on words typed in the editor");
    m_shortName = wxT("Word Completion");

    m_thread = new WordCompletionThread(this);
    m_thread->Start();

    wxTheApp->Connect(XRCID("text_word_complete"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(WordCompletionPlugin::OnWordComplete),
                      NULL,
                      this);
    wxTheApp->Connect(XRCID("text_word_complete_settings"),
                      wxEVT_COMMAND_MENU_SELECTED,
                      wxCommandEventHandler(WordCompletionPlugin::OnSettings),
                      NULL,
                      this);
    EventNotifier::Get()->Connect(
        wxEVT_EDITOR_INITIALIZING, clCommandEventHandler(WordCompletionPlugin::OnEditorHandler), NULL, this);
    clKeyboardManager::Get()->AddGlobalAccelerator(
        "text_word_complete", "Ctrl-ENTER", "Plugins::Word Completion::Show word completion");
}

WordCompletionPlugin::~WordCompletionPlugin() {}

clToolBar* WordCompletionPlugin::CreateToolBar(wxWindow* parent)
{
    wxUnusedVar(parent);
    clToolBar* tb(NULL);
    return tb;
}

void WordCompletionPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu;
    menu->Append(XRCID("text_word_complete"), _("Show Word Completion"));
    menu->AppendSeparator();
    menu->Append(XRCID("text_word_complete_settings"), _("Settings"));
    pluginsMenu->Append(wxID_ANY, GetShortName(), menu);
}

void WordCompletionPlugin::HookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void WordCompletionPlugin::UnHookPopupMenu(wxMenu* menu, MenuType type)
{
    wxUnusedVar(menu);
    wxUnusedVar(type);
}

void WordCompletionPlugin::UnPlug()
{
    m_thread->Stop();   // Stop the thread
    wxDELETE(m_thread); // Delete it

    wxTheApp->Disconnect(XRCID("text_word_complete"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(WordCompletionPlugin::OnWordComplete),
                         NULL,
                         this);
    wxTheApp->Disconnect(XRCID("text_word_complete_settings"),
                         wxEVT_COMMAND_MENU_SELECTED,
                         wxCommandEventHandler(WordCompletionPlugin::OnSettings),
                         NULL,
                         this);
    EventNotifier::Get()->Disconnect(
        wxEVT_EDITOR_INITIALIZING, clCommandEventHandler(WordCompletionPlugin::OnEditorHandler), NULL, this);
}

void WordCompletionPlugin::OnSuggestThread(const WordCompletionThreadReply& reply)
{
    IEditor* activeEditor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(activeEditor);

    // File has been modified since we triggered the completion request
    if(activeEditor->GetFileName() != reply.filename) return;

    // Build the suggetsion list
    wxString suggestString;
    for(wxStringSet_t::iterator iter = reply.suggest.begin(); iter != reply.suggest.end(); ++iter) {
        suggestString << *iter << "?1 ";
    }
    if(!suggestString.IsEmpty()) {
        suggestString.RemoveLast();
    }

    // Auto insert single match
#ifndef __WXGTK__
    // On GTK, AutoCompSetChooseSingle causes a mess when inserting the match
    activeEditor->GetSTC()->AutoCompSetChooseSingle(true);
#endif
    activeEditor->GetSTC()->AutoCompSetAutoHide(true);
    activeEditor->GetSTC()->AutoCompShow(reply.filter.length(), suggestString);
}

void WordCompletionPlugin::OnWordComplete(wxCommandEvent& event)
{
    event.Skip();
    IEditor* activeEditor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(activeEditor);

    // If the code completion box is shown, hide it
    if(activeEditor->IsCompletionBoxShown()) {
        activeEditor->HideCompletionBox();
    }

    wxStyledTextCtrl* stc = activeEditor->GetSTC();
    int curPos = stc->GetCurrentPos();
    int start = stc->WordStartPosition(stc->GetCurrentPos(), true);
    if(curPos <= start) return;

    wxString filter = stc->GetTextRange(start, curPos);
    // Invoke the thread to parse and suggets words for this file
    WordCompletionThreadRequest* req = new WordCompletionThreadRequest;
    req->buffer = stc->GetText();
    req->filename = activeEditor->GetFileName();
    req->filter = filter;
    m_thread->Add(req);
}

void WordCompletionPlugin::OnSettings(wxCommandEvent& event)
{
    WordCompletionSettingsDlg dlg(EventNotifier::Get()->TopFrame());
    dlg.ShowModal();
}

void WordCompletionPlugin::OnEditorHandler(clCommandEvent& event)
{
    event.Skip();
    wxStyledTextCtrl* editor = reinterpret_cast<wxStyledTextCtrl*>(event.GetEventObject());
    if(editor) {
        editor->RegisterImage(1, m_images.Bitmap("m_bmpWord"));
    }
}
