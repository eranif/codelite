#include "ChatAIWindow.hpp"

#include "ChatAI.hpp"
#include "ChatAISettingsDlg.hpp"
#include "ColoursAndFontsManager.h"
#include "OllamaClient.hpp"
#include "clSTCHelper.hpp"
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
}

ChatAIWindow::ChatAIWindow(wxWindow* parent, ChatAI* plugin)
    : AssistanceAIChatWindowBase(parent)
    , m_plugin(plugin)
{
    auto images = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(wxID_PREFERENCES, _("Settings"), images->Add("cog"));
    m_toolbar->AddTool(wxID_CLEAR, _("Clear content"), images->Add("clear"));
    m_activeModel = new wxChoice(m_toolbar, wxID_ANY);
    m_toolbar->AddControl(m_activeModel);
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(wxID_EXECUTE, _("Launch local model"), images->Add("run"));
    m_toolbar->AddTool(wxID_STOP, _("Stop"), images->Add("stop"));
    m_toolbar->AddTool(XRCID("restart-model"), _("Restart - Ctrl-Shift-ENTER"), images->Add("debugger_restart"));
    PopulateModels();
    m_toolbar->Realize();

    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &ChatAIWindow::OnUpdateTheme, this);

    EventNotifier::Get()->Bind(wxEVT_LLAMACLI_STARTED, &ChatAIWindow::OnChatAIStarted, this);
    EventNotifier::Get()->Bind(wxEVT_LLAMACLI_STDOUT, &ChatAIWindow::OnChatAIOutput, this);
    EventNotifier::Get()->Bind(wxEVT_LLAMACLI_STDERR, &ChatAIWindow::OnChatAIStderr, this);
    EventNotifier::Get()->Bind(wxEVT_LLAMACLI_TERMINATED, &ChatAIWindow::OnChatAITerminated, this);
    m_stcInput->Bind(wxEVT_KEY_DOWN, &ChatAIWindow::OnKeyDown, this);
    m_stcOutput->Bind(wxEVT_KEY_DOWN, &ChatAIWindow::OnKeyDown, this);
    Bind(wxEVT_MENU, &ChatAIWindow::OnSettings, this, wxID_PREFERENCES);
    Bind(wxEVT_MENU, &ChatAIWindow::OnClear, this, wxID_CLEAR);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnStopModelUI, this, wxID_STOP);
    Bind(wxEVT_MENU, &ChatAIWindow::OnStopModel, this, wxID_STOP);
    Bind(wxEVT_MENU, &ChatAIWindow::OnStartModel, this, wxID_EXECUTE);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnStartModelUI, this, wxID_EXECUTE);
    Bind(wxEVT_MENU, &ChatAIWindow::OnRestartModel, this, XRCID("restart-model"));
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnRestartModelUI, this, XRCID("restart-model"));
    m_activeModel->Bind(wxEVT_CHOICE, &ChatAIWindow::OnActiveModelChanged, this);
    m_stcInput->CmdKeyClear('R', wxSTC_KEYMOD_CTRL);
}

ChatAIWindow::~ChatAIWindow()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &ChatAIWindow::OnUpdateTheme, this);
    EventNotifier::Get()->Unbind(wxEVT_LLAMACLI_STARTED, &ChatAIWindow::OnChatAIStarted, this);
    EventNotifier::Get()->Unbind(wxEVT_LLAMACLI_STDOUT, &ChatAIWindow::OnChatAIOutput, this);
    EventNotifier::Get()->Unbind(wxEVT_LLAMACLI_STDERR, &ChatAIWindow::OnChatAIStderr, this);
    EventNotifier::Get()->Unbind(wxEVT_LLAMACLI_TERMINATED, &ChatAIWindow::OnChatAITerminated, this);
}

void ChatAIWindow::OnSend(wxCommandEvent& event)
{
    wxUnusedVar(event);
    SendPromptEvent();
}

void ChatAIWindow::SendPromptEvent()
{
    clCommandEvent sendEvent{ wxEVT_CHATAI_SEND };
    sendEvent.SetString(m_stcInput->GetText());
    EventNotifier::Get()->AddPendingEvent(sendEvent);
}

void ChatAIWindow::OnSendUI(wxUpdateUIEvent& event) { event.Enable(m_llamaCliRunning && !m_stcInput->IsEmpty()); }

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
    m_stcInput->SetCaretStyle(wxSTC_CARETSTYLE_BLOCK);
    m_stcOutput->SetCaretStyle(wxSTC_CARETSTYLE_BLOCK);
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
        if (win && win == m_stcInput) {
            wxCommandEvent dummy;
            OnRestartModel(dummy);
        } else {
            event.Skip();
        }
    } break;
    case WXK_RETURN:
    case WXK_NUMPAD_ENTER:
        if (win && win == m_stcInput && event.GetModifiers() == wxMOD_SHIFT) {
            // Send the command
            SendPromptEvent();
        } else if (win && win == m_stcInput && event.RawControlDown() && event.ShiftDown() && !event.AltDown()) {
            // Restart
            wxCommandEvent dummy;
            OnRestartModel(dummy);
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
    ShowSettings();
}

void ChatAIWindow::OnClear(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_stcOutput->ClearAll();
}

void ChatAIWindow::OnStartModel(wxCommandEvent& event)
{
    wxUnusedVar(event);
    auto activeModelName =
        m_plugin->GetConfig().GetSelectedModel() ? m_plugin->GetConfig().GetSelectedModel()->m_name : wxString();
    AppendOutputText(wxString() << _("Starting model...") << activeModelName << "\n");
    clCommandEvent start_event{ wxEVT_CHATAI_START };
    EventNotifier::Get()->AddPendingEvent(start_event);
}

void ChatAIWindow::OnStopModel(wxCommandEvent& event)
{
    wxUnusedVar(event);
    clCommandEvent event_stop{ wxEVT_CHATAI_STOP };
    EventNotifier::Get()->AddPendingEvent(event_stop);
}

void ChatAIWindow::ShowSettings()
{
    ChatAISettingsDlg dlg(this, m_plugin->GetConfig());
    if (dlg.ShowModal() == wxID_OK) {
        PopulateModels();
    }
}

void ChatAIWindow::OnChatAIStarted(clCommandEvent& event)
{
    wxUnusedVar(event);
    m_llamaCliRunning = true;
}

void ChatAIWindow::OnChatAITerminated(clCommandEvent& event)
{
    wxUnusedVar(event);
    m_llamaCliRunning = false;
    AppendOutputText("\n---\n");
    m_stcInput->Enable(true);
    m_stcInput->CallAfter(&wxStyledTextCtrl::SetFocus);

    if (m_autoRestart) {
        // Restarting the model
        wxCommandEvent dummy;
        OnStartModel(dummy);
    }
    m_autoRestart = false;
}

void ChatAIWindow::OnChatAIOutput(clCommandEvent& event)
{
    wxUnusedVar(event);
    AppendOutputText(event.GetString());
}

void ChatAIWindow::OnChatAIStderr(clCommandEvent& event)
{
    ::wxMessageBox(event.GetString(), "CodeLite", wxOK | wxCENTER | wxICON_ERROR);
    clERROR() << "ChatAI:" << event.GetString() << endl;
}

void ChatAIWindow::OnInputUI(wxUpdateUIEvent& event) { event.Enable(m_llamaCliRunning); }
void ChatAIWindow::PopulateModels()
{
    m_activeModel->Clear();
    auto models = m_plugin->GetConfig().GetModels();
    for (auto model : models) {
        m_activeModel->Append(model->m_name);
    }

    auto activeModelName =
        m_plugin->GetConfig().GetSelectedModel() ? m_plugin->GetConfig().GetSelectedModel()->m_name : wxString();
    if (!activeModelName.empty()) {
        m_activeModel->SetStringSelection(activeModelName);
    }
}

void ChatAIWindow::OnActiveModelChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString activeModel = m_activeModel->GetStringSelection();
    m_plugin->GetConfig().SetSelectedModelName(activeModel);
    m_plugin->GetConfig().Save();

    if (m_llamaCliRunning) {
        // A model is running, prompt the user to switch
        wxString message;
        message << _("Would you also like to change the running model to: ") << activeModel << " ?";

        CallAfter(&ChatAIWindow::PromptForModelReplace, message);
    }
}

void ChatAIWindow::SetFocusToActiveEditor()
{
    auto editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);
    editor->SetActive();
}

void ChatAIWindow::OnStopModelUI(wxUpdateUIEvent& event) { event.Enable(m_llamaCliRunning); }
void ChatAIWindow::OnStartModelUI(wxUpdateUIEvent& event) { event.Enable(!m_llamaCliRunning); }

void ChatAIWindow::OnRestartModel(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxBusyCursor bc;
    clCommandEvent stop_event{ wxEVT_CHATAI_STOP };
    EventNotifier::Get()->AddPendingEvent(stop_event);

    m_autoRestart = true;
}

void ChatAIWindow::OnRestartModelUI(wxUpdateUIEvent& event) { event.Enable(m_llamaCliRunning); }

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

void ChatAIWindow::PromptForModelReplace(const wxString& message)
{
    if (::wxMessageBox(message, "CodeLite", wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxICON_QUESTION, nullptr) !=
        wxYES) {
        return;
    }

    wxCommandEvent dummy;
    OnRestartModel(dummy);
}
