#include "ChatAIWindow.hpp"

#include "ChatAI.hpp"
#include "ColoursAndFontsManager.h"
#include "OllamaClient.hpp"
#include "StringUtils.h"
#include "clSTCHelper.hpp"
#include "clWorkspaceManager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"

#include <wx/msgdlg.h>

wxDEFINE_EVENT(wxEVT_CHATAI_SEND, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CHATAI_STOP, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CHATAI_START, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CHATAI_INTERRUPT, clCommandEvent);

namespace
{
const wxString CHAT_AI_LABEL = _("Chat AI");
const wxString LONG_MODEL_NAME = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
constexpr const char* kAssistantConfigFile = "assistant.json";
} // namespace

ChatAIWindow::ChatAIWindow(wxWindow* parent, ChatAI* plugin)
    : AssistanceAIChatWindowBase(parent)
    , m_plugin(plugin)
{
    auto images = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(wxID_CLEAR, _("Clear chat"), images->Add("clear"));
    m_activeModel = new wxChoice(
        m_toolbar, wxID_ANY, wxDefaultPosition, wxSize(GetTextExtent(LONG_MODEL_NAME).GetWidth(), wxNOT_FOUND));
    m_toolbar->AddControl(m_activeModel);
    m_toolbar->AddTool(wxID_REFRESH, _("Load models list"), images->Add("debugger_restart"));
    m_toolbar->AddTool(wxID_SETUP, _("Settings"), images->Add("cog"));
    m_toolbar->Realize();

    PopulateModels();
    m_activeModel->Bind(wxEVT_CHOICE, &ChatAIWindow::OnModelChanged, this);

    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &ChatAIWindow::OnUpdateTheme, this);
    EventNotifier::Get()->Bind(wxEVT_OLLAMA_OUTPUT, &ChatAIWindow::OnChatAIOutput, this);
    EventNotifier::Get()->Bind(wxEVT_OLLAMA_CHAT_DONE, &ChatAIWindow::OnChatAIOutputDone, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &ChatAIWindow::OnFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_OLLAMA_LIST_MODELS, &ChatAIWindow::OnModels, this);

    m_stcInput->Bind(wxEVT_KEY_DOWN, &ChatAIWindow::OnKeyDown, this);
    m_stcOutput->Bind(wxEVT_KEY_DOWN, &ChatAIWindow::OnKeyDown, this);
    Bind(wxEVT_MENU, &ChatAIWindow::OnClear, this, wxID_CLEAR);
    Bind(wxEVT_MENU, &ChatAIWindow::OnRefreshModelList, this, wxID_REFRESH);
    Bind(wxEVT_MENU, &ChatAIWindow::OnSettings, this, wxID_SETUP);
    m_stcInput->CmdKeyClear('R', wxSTC_KEYMOD_CTRL);

    m_plugin->GetClient().SetLogSink([]([[maybe_unused]] ollama::LogLevel level, std::string message) {
        // For now, just print it to the log.
        clDEBUG() << message << endl;
    });
}

ChatAIWindow::~ChatAIWindow()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &ChatAIWindow::OnUpdateTheme, this);
    EventNotifier::Get()->Unbind(wxEVT_OLLAMA_OUTPUT, &ChatAIWindow::OnChatAIOutput, this);
    EventNotifier::Get()->Unbind(wxEVT_OLLAMA_CHAT_DONE, &ChatAIWindow::OnChatAIOutputDone, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &ChatAIWindow::OnFileSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_OLLAMA_LIST_MODELS, &ChatAIWindow::OnModels, this);
}

void ChatAIWindow::OnSend(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSendPrompt();
}

void ChatAIWindow::DoSendPrompt()
{
    wxBusyCursor bc{};
    m_plugin->GetClient().Send(m_stcInput->GetText(), m_activeModel->GetStringSelection());
    m_stcInput->ClearAll();
}

void ChatAIWindow::OnSendUI(wxUpdateUIEvent& event) { event.Enable(!m_plugin->GetClient().IsBusy()); }

void ChatAIWindow::OnModelChanged(wxCommandEvent& event)
{
    m_plugin->GetConfig().SetSelectedModelName(m_activeModel->GetStringSelection());
    m_plugin->GetConfig().Save();
}

void ChatAIWindow::OnUpdateTheme(wxCommandEvent& event)
{
    event.Skip();
    UpdateTheme();
}

void ChatAIWindow::UpdateTheme()
{
    auto lexer = ColoursAndFontsManager::Get().GetLexer("markdown");
    CHECK_PTR_RET(lexer);

    lexer->Apply(m_stcInput);
    lexer->Apply(m_stcOutput);
}

void ChatAIWindow::OnKeyDown(wxKeyEvent& event)
{
    wxWindow* win = dynamic_cast<wxWindow*>(event.GetEventObject());
    switch (event.GetKeyCode()) {
    case WXK_ESCAPE: {
        clGetManager()->ShowManagementWindow(CHAT_AI_LABEL, false);
        auto editor = clGetManager()->GetActiveEditor();
        CHECK_PTR_RET(editor);

        // Set the focus to the active editor
        CallAfter(&ChatAIWindow::SetFocusToActiveEditor);

    } break;
    case WXK_CONTROL_R: {
        event.Skip();
    } break;
    case WXK_RETURN:
    case WXK_NUMPAD_ENTER:
        if (win && win == m_stcInput && event.GetModifiers() == wxMOD_SHIFT) {
            // Send the command
            DoSendPrompt();
        } else {
            event.Skip();
        }
        break;
    default:
        event.Skip();
        break;
    }
}

void ChatAIWindow::OnSettings(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if (clWorkspaceManager::Get().IsWorkspaceOpened()) {
        auto editor = clWorkspaceManager::Get().GetWorkspace()->CreateOrOpenSettingFile(kAssistantConfigFile);
        if (!editor) {
            ::wxMessageBox(
                wxString() << _("Could not open file: assistant.jon"), "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
        }

        if (!editor->GetEditorText().empty()) {
            return;
        }

        // Place some default content
        editor->SetEditorText(
            R"#({
  "history_size": 50,
  "server_url": "http://127.0.0.1:11434",
  "servers": {},
  "models": {
    "default": {
      "options": {
        "num_ctx": 32768,
        "temperature": 0
      }
    }
  }
})#");
    }
}

void ChatAIWindow::OnRefreshModelList(wxCommandEvent& event)
{
    wxUnusedVar(event);
    PopulateModels();
}

void ChatAIWindow::OnClear(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_stcOutput->ClearAll();
    m_stcInput->ClearAll();
    m_plugin->GetClient().Clear();
}

void ChatAIWindow::OnChatAIOutput(OllamaEvent& event)
{
    if (event.GetReason() == ollama::Reason::kFatalError) {
        ::wxMessageBox(event.GetOutput(), "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
        return;
    }
    AppendOutputText(event.GetOutput());
}

void ChatAIWindow::OnFileSaved(clCommandEvent& event)
{
    // Always call Skip()
    event.Skip();
    CHECK_COND_RET(clWorkspaceManager::Get().IsWorkspaceOpened());
    CHECK_PTR_RET(clGetManager()->GetActiveEditor());

    wxString filepath = clGetManager()->GetActiveEditor()->GetRemotePathOrLocal();
    if (filepath == clWorkspaceManager::Get().GetWorkspace()->GetSettingFileFullPath(kAssistantConfigFile)) {
        // Reload configuration
        m_plugin->GetClient().ReloadConfig(clGetManager()->GetActiveEditor()->GetEditorText());
    }
}

void ChatAIWindow::OnModels(OllamaEvent& event)
{
    m_activeModel->Clear();
    for (const auto& model : event.GetModels()) {
        m_activeModel->Append(model);
    }

    const auto& active_model = m_plugin->GetConfig().GetSelectedModel();
    if (!active_model.empty()) {
        m_activeModel->SetStringSelection(active_model);
    } else if (!event.GetModels().empty()) {
        m_activeModel->SetSelection(0);
    }
}

void ChatAIWindow::OnChatAIOutputDone(OllamaEvent& event)
{
    AppendOutputText("\n------\n");

    // Move the focus back to the input text control
    m_stcInput->CallAfter(&wxStyledTextCtrl::SetFocus);
}

void ChatAIWindow::OnInputUI(wxUpdateUIEvent& event) { event.Enable(true); }

void ChatAIWindow::PopulateModels()
{
    wxBusyCursor bc{};
    m_plugin->GetClient().GetModels();
}

void ChatAIWindow::SetFocusToActiveEditor()
{
    auto editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);
    editor->SetActive();
}

void ChatAIWindow::AppendOutputText(const wxString& message)
{
    bool scroll_to_end = true;
    if (wxWindow::FindFocus() == m_stcOutput &&
        m_stcOutput->GetCurrentLine() != m_stcOutput->LineFromPosition(m_stcOutput->GetLastPosition())) {
        scroll_to_end = false;
    }

    m_stcOutput->AppendText(message);
    if (scroll_to_end) {
        m_stcOutput->ScrollToEnd();
        clSTCHelper::SetCaretAt(m_stcOutput, m_stcOutput->GetLastPosition());
    }
}
