#include "ChatAIWindow.hpp"

#include "ChatAISettingsDlg.hpp"
#include "ColoursAndFontsManager.h"
#include "LLAMCli.hpp"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"

wxDEFINE_EVENT(wxEVT_CHATAI_SEND, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CHATAI_STOP, clCommandEvent);

namespace
{
const wxString CHAT_AI_LABEL = _("Chat AI");
}

ChatAIWindow::ChatAIWindow(wxWindow* parent, ChatAIConfig& config)
    : AssistanceAIChatWindowBase(parent)
    , m_config(config)
{
    auto images = clGetManager()->GetStdIcons();

    m_toolbar->AddTool(wxID_PREFERENCES, _("Settings"), images->LoadBitmap("cog", 24));
    m_toolbar->AddTool(wxID_CLEAR, _("Clear content"), images->LoadBitmap("clear", 24));
    m_activeModel = new wxChoice(m_toolbar, wxID_ANY);
    m_toolbar->AddControl(m_activeModel);
    PopulateModels();

    m_toolbar->SetToolBitmapSize(FromDIP(wxSize(24, 24)));
    m_toolbar->Realize();

    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &ChatAIWindow::OnUpdateTheme, this);

    EventNotifier::Get()->Bind(wxEVT_LLAMACLI_STARTED, &ChatAIWindow::OnChatAIStarted, this);
    EventNotifier::Get()->Bind(wxEVT_LLAMACLI_STDOUT, &ChatAIWindow::OnChatAIOutput, this);
    EventNotifier::Get()->Bind(wxEVT_LLAMACLI_STDERR, &ChatAIWindow::OnChatAIStderr, this);
    EventNotifier::Get()->Bind(wxEVT_LLAMACLI_TERMINATED, &ChatAIWindow::OnChatAITerminated, this);
    m_stcInput->Bind(wxEVT_KEY_DOWN, &ChatAIWindow::OnKeyDown, this);
    Bind(wxEVT_MENU, &ChatAIWindow::OnSettings, this, wxID_PREFERENCES);
    Bind(wxEVT_MENU, &ChatAIWindow::OnClear, this, wxID_CLEAR);
    m_activeModel->Bind(wxEVT_CHOICE, &ChatAIWindow::OnActiveModelChanged, this);
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

void ChatAIWindow::OnSendUI(wxUpdateUIEvent& event) { event.Enable(!m_llamaCliRunning && !m_stcInput->IsEmpty()); }

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
    switch (event.GetKeyCode()) {
    case WXK_ESCAPE: {
        clGetManager()->ShowManagementWindow(CHAT_AI_LABEL, false);
        auto editor = clGetManager()->GetActiveEditor();
        CHECK_PTR_RET(editor);

        // Set the focus to the active editor
        editor->GetCtrl()->CallAfter(&wxStyledTextCtrl::SetFocus);

    } break;
    case WXK_RETURN:
    case WXK_NUMPAD_ENTER:
        if (event.GetModifiers() == wxMOD_SHIFT) {
            // Send the command
            SendPromptEvent();
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

void ChatAIWindow::ShowSettings()
{
    ChatAISettingsDlg dlg(this, m_config);
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
    m_stcOutput->AppendText("\n----\n");
    m_stcOutput->ScrollToEnd();
    m_stcInput->Enable(true);
    m_stcInput->CallAfter(&wxStyledTextCtrl::SetFocus);
}

void ChatAIWindow::OnChatAIOutput(clCommandEvent& event)
{
    wxUnusedVar(event);
    m_stcOutput->AppendText(event.GetString());
    m_stcOutput->ScrollToEnd();
}

void ChatAIWindow::OnChatAIStderr(clCommandEvent& event)
{
    ::wxMessageBox(event.GetString(), "CodeLite", wxOK | wxCENTER | wxICON_ERROR);
    clERROR() << "ChatAI:" << event.GetString() << endl;
}

void ChatAIWindow::OnInputUI(wxUpdateUIEvent& event) { event.Enable(!m_llamaCliRunning); }
void ChatAIWindow::OnStopUI(wxUpdateUIEvent& event) { event.Enable(m_llamaCliRunning); }
void ChatAIWindow::OnStop(wxCommandEvent& event)
{
    wxUnusedVar(event);
    clCommandEvent event_stop{ wxEVT_CHATAI_STOP };
    EventNotifier::Get()->AddPendingEvent(event_stop);
}

void ChatAIWindow::PopulateModels()
{
    m_activeModel->Clear();
    auto models = m_config.GetModels();
    for (auto model : models) {
        m_activeModel->Append(model->m_name);
    }

    auto activeModelName = m_config.GetSelectedModel() ? m_config.GetSelectedModel()->m_name : wxString();
    if (!activeModelName.empty()) {
        m_activeModel->SetStringSelection(activeModelName);
    }
}

void ChatAIWindow::OnActiveModelChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString activeModel = m_activeModel->GetStringSelection();
    m_config.SetSelectedModelName(activeModel);
    m_config.Save();
}