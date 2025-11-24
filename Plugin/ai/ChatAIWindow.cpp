#include "ChatAIWindow.hpp"

#include "ChatAI.hpp"
#include "ChatAIWindowFrame.hpp"
#include "ChatHistoryDialog.hpp"
#include "ColoursAndFontsManager.h"
#include "MarkdownStyler.hpp"
#include "ai/LLMManager.hpp"
#include "aui/clAuiToolBarArt.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "clSTCHelper.hpp"
#include "clWorkspaceManager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"

#include <algorithm>
#include <wx/msgdlg.h>

namespace
{
const wxString CHAT_AI_LABEL = _("Chat AI");
const wxString LONG_MODEL_NAME = "claude-sonnet-4-5-1234567890";
} // namespace

ChatAIWindow::ChatAIWindow(wxWindow* parent, ChatAI* plugin)
    : AssistanceAIChatWindowBase(parent)
    , m_plugin(plugin)
{
    auto images = clGetManager()->GetStdIcons();
    m_toolbar->SetArtProvider(new clAuiToolBarArt());
    clAuiToolBarArt::AddTool(m_toolbar, wxID_CLEAR, _("Clear the chat history"), images->LoadBitmap("clear"));
    m_toolbar->AddSeparator();

    wxSize control_size{GetTextExtent(LONG_MODEL_NAME).GetWidth(), wxNOT_FOUND};
    m_choiceEndpoints = new wxChoice(m_toolbar, wxID_ANY, wxDefaultPosition, control_size);
    m_choiceEndpoints->SetToolTip(_("Choose the endpoint to use"));
    m_toolbar->AddControl(m_choiceEndpoints);

    m_choicePlaceHolders = new wxChoice(m_toolbar, wxID_ANY, wxDefaultPosition, control_size);
    m_choicePlaceHolders->SetToolTip(
        _("Insert a placeholder into the prompt. Alternatively, type '{' to invoke auto-completion."));
    m_toolbar->AddControl(m_choicePlaceHolders);

    m_checkboxEnableTools = new wxCheckBox(m_toolbar, wxID_ANY, _("Enable Tools"));
    bool enable_tools = clConfig::Get().Read("chat-ai/enable-tools", true);
    m_checkboxEnableTools->SetValue(enable_tools);
    m_checkboxEnableTools->SetToolTip(_("Enable Local MCP Tools for the LLM"));
    m_toolbar->AddControl(m_checkboxEnableTools);
    m_toolbar->AddSeparator();
    clAuiToolBarArt::AddTool(m_toolbar, wxID_EXECUTE, _("Submit"), images->LoadBitmap("run"));
    clAuiToolBarArt::AddTool(m_toolbar, wxID_STOP, _("Stop"), images->LoadBitmap("execute_stop"));
    m_toolbar->AddSeparator();
    clAuiToolBarArt::AddTool(m_toolbar, wxID_REFRESH, _("Restart the client"), images->LoadBitmap("debugger_restart"));
    clAuiToolBarArt::AddTool(
        m_toolbar, XRCID("prompt_history"), _("Show prompt history"), images->LoadBitmap("history"));
    clAuiToolBarArt::AddTool(m_toolbar,
                             XRCID("auto_scroll"),
                             _("Enable auto scrolling"),
                             images->LoadBitmap("link_editor"),
                             wxEmptyString,
                             wxITEM_CHECK);

    m_toolbar->AddSeparator();
    if (IsDetached()) {
        clAuiToolBarArt::AddTool(
            m_toolbar, XRCID("detach_view"), _("Dock the chat window"), images->LoadBitmap("merge-window"));

    } else {
        clAuiToolBarArt::AddTool(m_toolbar,
                                 XRCID("detach_view"),
                                 _("Move the chat into a separate window"),
                                 images->LoadBitmap("separate-window"));
    }

    clAuiToolBarArt::Finalise(m_toolbar);
    m_toolbar->Realize();

    m_choiceEndpoints->Bind(wxEVT_CHOICE, &ChatAIWindow::OnEndpointChanged, this);
    m_choicePlaceHolders->Bind(wxEVT_CHOICE, &ChatAIWindow::OnInsertPlaceHolder, this);

    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &ChatAIWindow::OnUpdateTheme, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &ChatAIWindow::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &ChatAIWindow::OnWorkspaceClosed, this);

    /// LLM events
    Bind(wxEVT_LLM_CHAT_STARTED, &ChatAIWindow::OnChatStarted, this);
    Bind(wxEVT_LLM_OUTPUT, &ChatAIWindow::OnChatAIOutput, this);
    Bind(wxEVT_LLM_OUTPUT_DONE, &ChatAIWindow::OnChatAIOutputDone, this);
    Bind(wxEVT_LLM_THINK_SATRTED, &ChatAIWindow::OnThinkingStart, this);
    Bind(wxEVT_LLM_THINK_ENDED, &ChatAIWindow::OnThinkingEnd, this);
    llm::Manager::GetInstance().Bind(wxEVT_LLM_CONFIG_UPDATED, &ChatAIWindow::OnLLMConfigUpdate, this);

    m_stcInput->Bind(wxEVT_KEY_DOWN, &ChatAIWindow::OnKeyDown, this);
    m_stcOutput->Bind(wxEVT_KEY_DOWN, &ChatAIWindow::OnKeyDown, this);
    m_stcOutput->SetCodePage(wxSTC_CP_UTF8);
    m_stcInput->SetCodePage(wxSTC_CP_UTF8);
    m_stcOutput->SetReadOnly(true);

    Bind(wxEVT_MENU, &ChatAIWindow::OnNewSession, this, wxID_CLEAR);
    Bind(wxEVT_MENU, &ChatAIWindow::OnRestartClient, this, wxID_REFRESH);
    Bind(wxEVT_MENU, &ChatAIWindow::OnSend, this, wxID_EXECUTE);
    Bind(wxEVT_MENU, &ChatAIWindow::OnStop, this, wxID_STOP);
    Bind(wxEVT_MENU, &ChatAIWindow::OnAutoScroll, this, XRCID("auto_scroll"));
    Bind(wxEVT_MENU, &ChatAIWindow::OnHistory, this, XRCID("prompt_history"));
    Bind(wxEVT_MENU, &ChatAIWindow::OnDetachView, this, XRCID("detach_view"));

    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnDetachViewUI, this, XRCID("detach_view"));
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnSendUI, this, wxID_EXECUTE);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnStopUI, this, wxID_STOP);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnClearOutputViewUI, this, wxID_CLEAR);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnAutoScrollUI, this, XRCID("auto_scroll"));
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnHistoryUI, this, XRCID("prompt_history"));
    m_choiceEndpoints->Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnBusyUI, this);
    m_choicePlaceHolders->Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnBusyUI, this);
    UpdateChoices();

    m_stcInput->CmdKeyClear('R', wxSTC_KEYMOD_CTRL);
    m_stcInput->Bind(wxEVT_STC_CHARADDED, &ChatAIWindow::OnCharAdded, this);

    m_markdownStyler = std::make_unique<MarkdownStyler>(m_stcOutput);
    m_markdownStyler->Bind(wxEVT_MARKDOWN_LINK_CLICKED, [](clCommandEvent& event) {
        wxString url = event.GetString();
        ::wxLaunchDefaultBrowser(url);
    });

    m_statusPanel = new IndicatorPanel(this);
    GetSizer()->Add(m_statusPanel, wxSizerFlags(0).Expand());

    CallAfter(&ChatAIWindow::RestoreUI);
    ShowIndicator(false);

    m_cancel_token = std::make_shared<llm::CancellationToken>();
}

ChatAIWindow::~ChatAIWindow()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &ChatAIWindow::OnUpdateTheme, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &ChatAIWindow::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &ChatAIWindow::OnWorkspaceClosed, this);

    /// LLM events
    Unbind(wxEVT_LLM_CHAT_STARTED, &ChatAIWindow::OnChatStarted, this);
    Unbind(wxEVT_LLM_OUTPUT, &ChatAIWindow::OnChatAIOutput, this);
    Unbind(wxEVT_LLM_OUTPUT_DONE, &ChatAIWindow::OnChatAIOutputDone, this);
    Unbind(wxEVT_LLM_THINK_SATRTED, &ChatAIWindow::OnThinkingStart, this);
    Unbind(wxEVT_LLM_THINK_ENDED, &ChatAIWindow::OnThinkingEnd, this);
    llm::Manager::GetInstance().Unbind(wxEVT_LLM_CONFIG_UPDATED, &ChatAIWindow::OnLLMConfigUpdate, this);

    clConfig::Get().Write("chat-ai/sash-position", m_mainSplitter->GetSashPosition());
    clConfig::Get().Write("chat-ai/enable-tools", m_checkboxEnableTools->IsChecked());
}

void ChatAIWindow::OnSend(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSendPrompt();
}

void ChatAIWindow::DoSendPrompt()
{
    wxBusyCursor bc{};

    wxString selected_endpoint = m_choiceEndpoints->GetStringSelection();
    if (selected_endpoint.empty()) {
        ::wxMessageBox(_("Please choose an endpoint."), "CodeLite", wxICON_WARNING | wxOK | wxCENTRE);
        return;
    }

    wxString prompt = m_stcInput->GetText();
    prompt.Trim().Trim(false);
    m_cancel_token->Reset();

    llm::ChatOptions chat_options{llm::ChatOptions::kDefault};
    if (!m_checkboxEnableTools->IsChecked()) {
        // No tools.
        llm::AddFlagSet(chat_options, llm::ChatOptions::kNoTools);
    }
    llm::Manager::GetInstance().Chat(this, prompt, m_cancel_token, chat_options);

    // Remember this prompt in the history.
    if (!prompt.empty()) {
        llm::Manager::GetInstance().GetConfig().AddHistory(prompt);
        llm::Manager::GetInstance().GetConfig().Save();
    }

    prompt.Prepend(wxString() << "\n**" << ::wxGetUserId() << "**:\n");
    AppendOutput(prompt + "\n\n");
    m_stcInput->ClearAll();
    ShowIndicator(true);
}

void ChatAIWindow::OnClearOutputViewUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_stcOutput->IsEmpty() && !llm::Manager::GetInstance().IsBusy());
}

void ChatAIWindow::OnSendUI(wxUpdateUIEvent& event)
{
    wxString prompt = m_stcInput->GetText();
    prompt.Trim().Trim(false);
    event.Enable(!llm::Manager::GetInstance().IsBusy() && !prompt.empty() &&
                 !m_choiceEndpoints->GetStringSelection().empty());
}

void ChatAIWindow::OnInsertPlaceHolder(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString placeholder = m_choicePlaceHolders->GetStringSelection();
    m_stcInput->InsertText(m_stcInput->GetCurrentPos(), placeholder);
    clSTCHelper::SetCaretAt(m_stcInput, m_stcInput->GetCurrentPos() + placeholder.length());
    m_stcInput->CallAfter(&wxStyledTextCtrl::SetFocus);
}

void ChatAIWindow::OnEndpointChanged(wxCommandEvent& event)
{
    wxUnusedVar(event);
    llm::Manager::GetInstance().SetActiveEndpoint(m_choiceEndpoints->GetStringSelection());
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

    wxColour bg_colour;
    if (lexer->IsDark()) {
        bg_colour = lexer->GetProperty(0).GetBgColour();
        bg_colour = bg_colour.ChangeLightness(105);

    } else {
        bg_colour = lexer->GetProperty(0).GetBgColour();
    }

    for (int style = 0; style < wxSTC_STYLE_MAX; ++style) {
        m_stcInput->StyleSetBackground(style, bg_colour);
    }

    AnsiColours::SetDarkTheme(lexer->IsDark());
}

void ChatAIWindow::OnKeyDown(wxKeyEvent& event)
{
    wxWindow* win = dynamic_cast<wxWindow*>(event.GetEventObject());
    switch (event.GetKeyCode()) {
    case WXK_ESCAPE: {
        if (m_stcInput->AutoCompActive()) {
            m_stcInput->AutoCompCancel();
        } else {
            if (dynamic_cast<ChatAIWindowFrame*>(GetParent())) {
                // floating state
                GetParent()->Show(false);
            } else {
                clGetManager()->ShowManagementWindow(CHAT_AI_LABEL, false);
            }
            auto editor = clGetManager()->GetActiveEditor();
            CHECK_PTR_RET(editor);

            // Set the focus to the active editor
            CallAfter(&ChatAIWindow::SetFocusToActiveEditor);
        }
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

void ChatAIWindow::OnRestartClient(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_cancel_token->Cancel();

    // Reload configuration
    wxBusyCursor bc{};
    if (llm::Manager::GetInstance().ReloadConfig(std::nullopt, false)) {
        clGetManager()->SetStatusMessage(_("LLM client successfully restarted"), 3);
    }
}

void ChatAIWindow::DoRestart()
{
    DoClearOutputView();
    llm::Manager::GetInstance().Restart();
}

void ChatAIWindow::DoClearOutputView()
{
    m_stcOutput->SetReadOnly(false);
    m_stcOutput->ClearAll();
    m_stcOutput->SetReadOnly(true);
    m_markdownStyler = std::make_unique<MarkdownStyler>(m_stcOutput);
    m_markdownStyler->Bind(wxEVT_MARKDOWN_LINK_CLICKED, [](clCommandEvent& event) {
        wxString url = event.GetString();
        ::wxLaunchDefaultBrowser(url);
    });
    llm::Manager::GetInstance().ClearHistory();
}

void ChatAIWindow::OnNewSession(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoClearOutputView();
}

void ChatAIWindow::OnChatStarted(clLLMEvent& event)
{
    event.Skip();
    m_state = ChatState::kWorking;
    AppendOutput("**assistant**:\n");
    ShowIndicator(true);
}

void ChatAIWindow::OnThinkingStart(clLLMEvent& event)
{
    event.Skip();
    m_state = ChatState::kThinking;
    m_statusPanel->SetMessage(_("Thinking..."));
}

void ChatAIWindow::OnThinkingEnd(clLLMEvent& event)
{
    event.Skip();
    m_state = ChatState::kWorking;
    m_statusPanel->SetMessage(_("Working..."));
}

void ChatAIWindow::OnChatAIOutput(clLLMEvent& event)
{
    const std::string& str = event.GetResponseRaw();
    wxString content = wxString::FromUTF8(str);
    switch (m_state) {
    case ChatState::kThinking:
        break;
    case ChatState::kWorking:
    case ChatState::kReady:
        AppendOutput(content);
        break;
    }
}

void ChatAIWindow::OnChatAIOutputDone(clLLMEvent& event)
{
    wxUnusedVar(event);
    m_state = ChatState::kReady;

    if (m_cancel_token->IsCancelled()) {
        AppendOutput("\n\n** Generation cancelled by the user**\n");
    } else if (event.IsError()) {
        wxString message;
        message << "\n\n**An error occurred:**\n" << wxString::FromUTF8(event.GetResponseRaw());
        AppendOutput(message);
    }

    m_cancel_token->Reset();
    StyleOutput();
    ShowIndicator(false);
}

void ChatAIWindow::ShowIndicator(bool show)
{
    if (show) {
        m_statusPanel->Start(_("Working..."));
    } else {
        m_statusPanel->Stop(_("Ready"));
    }
}

void ChatAIWindow::OnLLMConfigUpdate(clLLMEvent& event)
{
    // Always call Skip()
    event.Skip();
    // Clear the output view
    DoClearOutputView();
    UpdateChoices();
}

void ChatAIWindow::OnInputUI(wxUpdateUIEvent& event) { event.Enable(true); }

void ChatAIWindow::UpdateChoices()
{
    m_choiceEndpoints->Clear();
    m_choiceEndpoints->Append(llm::Manager::GetInstance().ListEndpoints());
    auto active_endpoint = llm::Manager::GetInstance().GetActiveEndpoint();
    if (active_endpoint.has_value()) {
        m_choiceEndpoints->SetStringSelection(active_endpoint.value());
    }

    m_choicePlaceHolders->Clear();
    m_choicePlaceHolders->Append(llm::Manager::GetInstance().GetAvailablePlaceHolders());
    m_choicePlaceHolders->SetSelection(0);
}

void ChatAIWindow::SetFocusToActiveEditor()
{
    auto editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);
    editor->SetActive();
}

void ChatAIWindow::AppendMarker()
{
    m_stcOutput->SetReadOnly(false);
    m_stcOutput->AppendText("\n");
    m_stcOutput->SetReadOnly(true);
}

void ChatAIWindow::AppendOutput(const wxString& text)
{
    CHECK_COND_RET(!text.empty());

    m_stcOutput->SetReadOnly(false);
    m_stcOutput->SetInsertionPointEnd();
    m_stcOutput->SetSelection(m_stcOutput->GetLastPosition(), m_stcOutput->GetLastPosition());
    m_stcOutput->AppendText(text);
    m_stcOutput->SetReadOnly(true);

    if (m_autoScroll) {
        m_stcOutput->ScrollToEnd();
        clSTCHelper::SetCaretAt(m_stcOutput, m_stcOutput->GetLastPosition());
    }
}

void ChatAIWindow::StyleOutput()
{
    m_markdownStyler->StyleText();
    if (m_autoScroll) {
        m_stcOutput->ScrollToEnd();
        clSTCHelper::SetCaretAt(m_stcOutput, m_stcOutput->GetLastPosition());
    } else {
        m_stcOutput->ClearSelections();
    }
}

void ChatAIWindow::OnWorkspaceLoaded(clWorkspaceEvent& event) { event.Skip(); }

void ChatAIWindow::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    DoRestart();
}

void ChatAIWindow::RestoreUI()
{
    int sash_position = clConfig::Get().Read("chat-ai/sash-position", wxNOT_FOUND);
    if (sash_position != wxNOT_FOUND) {
        m_mainSplitter->SetSashPosition(sash_position, true);
        m_mainSplitter->UpdateSize();
    }
}

void ChatAIWindow::LoadGlobalConfig()
{
    wxBusyCursor bc{};
    llm::Manager::GetInstance().ReloadConfig(std::nullopt, false);
}

void ChatAIWindow::OnAutoScroll(wxCommandEvent& event) { m_autoScroll = event.IsChecked(); }

void ChatAIWindow::OnAutoScrollUI(wxUpdateUIEvent& event) { event.Check(m_autoScroll); }

void ChatAIWindow::OnBusyUI(wxUpdateUIEvent& event)
{
    event.Enable(llm::Manager::GetInstance().IsAvailable() && !llm::Manager::GetInstance().IsBusy());
}

void ChatAIWindow::OnDetachView(wxCommandEvent& event)
{
    wxUnusedVar(event);
    if (IsDetached()) {
        m_plugin->CallAfter(&ChatAI::DockView);
    } else {
        m_plugin->CallAfter(&ChatAI::DetachView, true);
    }
}

void ChatAIWindow::OnHistory(wxCommandEvent& event)
{
    ChatHistoryDialog dlg(EventNotifier::Get()->TopFrame());
    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    // Update the prompt field.
    m_stcInput->SetText(dlg.GetSelectedPrompt());
    m_stcInput->CallAfter(&wxStyledTextCtrl::SetFocus);
}

void ChatAIWindow::OnHistoryUI(wxUpdateUIEvent& event)
{
    const auto& config = llm::Manager::GetInstance().GetConfig();
    event.Enable(!config.GetHistory().IsEmpty() && m_state == ChatState::kReady);
}

void ChatAIWindow::OnStop(wxCommandEvent& event)
{
    event.Skip();
    llm::Manager::GetInstance().Restart();
}

void ChatAIWindow::OnStopUI(wxUpdateUIEvent& event) { event.Enable(llm::Manager::GetInstance().IsBusy()); }

void ChatAIWindow::OnDetachViewUI(wxUpdateUIEvent& event) { event.Enable(true); }

bool ChatAIWindow::IsDetached() const
{
    auto parent = dynamic_cast<const ChatAIWindowFrame*>(GetParent());
    return parent != nullptr;
}

void ChatAIWindow::Chat(const wxString& prompt)
{
    if (prompt.empty()) {
        return;
    }
    m_stcInput->SetText(prompt);
    DoSendPrompt();
}

void ChatAIWindow::OnCharAdded(wxStyledTextEvent& event)
{
    event.Skip();
    auto strings = llm::Manager::GetInstance().GetAvailablePlaceHolders();
    std::sort(strings.begin(), strings.end());
    static const wxString candidates = StringUtils::Join(strings, " ");
    if (event.GetKey() == '{') {
        // user typed "{{"
        m_stcInput->AutoCompShow(1, candidates);
    }
}
