#include "ChatAIWindow.hpp"

#include "ChatAI.hpp"
#include "ColoursAndFontsManager.h"
#include "FileManager.hpp"
#include "MarkdownStyler.hpp"
#include "ai/LLMManager.hpp"
#include "aui/clAuiToolBarArt.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "clModuleLogger.hpp"
#include "clSTCHelper.hpp"
#include "clSingleChoiceDialog.h"
#include "clWorkspaceManager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"

#include <wx/msgdlg.h>

const wxString kDefaultSettings =
    R"#({
  "history_size": 50,
  "mcp_servers": {},
  "log_level": "info",
  "stream": true,
  "keep_alive": "24h",
  "endpoints": {
    "http://127.0.0.1:11434": {
      "active": true,
      "http_headers": {
        "Host": "127.0.0.1"
      },
      "type": "ollama"
    }
  },
  "models": {
    "default": {
      "options": {
        "num_ctx": 16384,
        "temperature": 0
      },
      "think_end_tag": "</think>",
      "think_start_tag": "</think>"
    }
  }
})#";

constexpr const char* kAssistantConfigFile = "assistant.json";

namespace
{
const wxString CHAT_AI_LABEL = _("Chat AI");
const wxString LONG_MODEL_NAME = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

std::optional<wxString> GetGlobalSettings()
{
    wxFileName global_config{clStandardPaths::Get().GetUserDataDir(), kAssistantConfigFile};
    global_config.AppendDir("config");

    wxString global_content;
    if (!FileUtils::ReadFileContent(global_config, global_content)) {
        return std::nullopt;
    }
    return global_content;
}

} // namespace

ChatAIWindow::ChatAIWindow(wxWindow* parent)
    : AssistanceAIChatWindowBase(parent)
{
    auto images = clGetManager()->GetStdIcons();
    m_toolbar->SetArtProvider(new clAuiToolBarArt());
    m_toolbar->AddTool(wxID_CLEAR, _("Clear everything and start a new session"), images->LoadBitmap("clear"));
    m_toolbar->AddTool(wxID_SETUP, _("Settings"), images->LoadBitmap("cog"));
    m_toolbar->AddSeparator();
    m_activeModel = new wxChoice(m_toolbar, wxID_ANY, wxDefaultPosition, wxDefaultSize);
    m_activeModel->SetToolTip(_("Change model. Changing a model will also clear your chat history"));
    m_toolbar->AddControl(m_activeModel);
    m_toolbar->AddTool(wxID_REFRESH, _("Load models list"), images->LoadBitmap("debugger_restart"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(wxID_EXECUTE, _("Submit"), images->LoadBitmap("run"));
    m_toolbar->AddTool(wxID_STOP, _("Stop"), images->LoadBitmap("execute_stop"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("prompt_history"), _("Show prompt history"), images->LoadBitmap("history"));
    m_toolbar->AddTool(XRCID("auto_scroll"),
                       _("Enable auto scrolling"),
                       images->LoadBitmap("link_editor"),
                       wxEmptyString,
                       wxITEM_CHECK);
    clAuiToolBarArt::Finalise(m_toolbar);
    m_toolbar->Realize();

    m_activeModel->Bind(wxEVT_CHOICE, &ChatAIWindow::OnModelChanged, this);

    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &ChatAIWindow::OnUpdateTheme, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &ChatAIWindow::OnFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &ChatAIWindow::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &ChatAIWindow::OnWorkspaceClosed, this);

    /// LLM events
    Bind(wxEVT_LLM_CHAT_STARTED, &ChatAIWindow::OnChatStarted, this);
    Bind(wxEVT_LLM_OUTPUT, &ChatAIWindow::OnChatAIOutput, this);
    Bind(wxEVT_LLM_OUTPUT_DONE, &ChatAIWindow::OnChatAIOutputDone, this);
    Bind(wxEVT_LLM_THINK_SATRTED, &ChatAIWindow::OnThinkingStart, this);
    Bind(wxEVT_LLM_THINK_ENDED, &ChatAIWindow::OnThinkingEnd, this);
    Bind(wxEVT_LLM_MODELS_LOADED, &ChatAIWindow::OnModelsLoaded, this);

    m_stcInput->Bind(wxEVT_KEY_DOWN, &ChatAIWindow::OnKeyDown, this);
    m_stcOutput->Bind(wxEVT_KEY_DOWN, &ChatAIWindow::OnKeyDown, this);
    m_stcOutput->SetCodePage(wxSTC_CP_UTF8);
    m_stcInput->SetCodePage(wxSTC_CP_UTF8);
    m_stcOutput->SetReadOnly(true);

    Bind(wxEVT_MENU, &ChatAIWindow::OnNewSession, this, wxID_CLEAR);
    Bind(wxEVT_MENU, &ChatAIWindow::OnRefreshModelList, this, wxID_REFRESH);
    Bind(wxEVT_MENU, &ChatAIWindow::OnSettings, this, wxID_SETUP);
    Bind(wxEVT_MENU, &ChatAIWindow::OnSend, this, wxID_EXECUTE);
    Bind(wxEVT_MENU, &ChatAIWindow::OnStop, this, wxID_STOP);
    Bind(wxEVT_MENU, &ChatAIWindow::OnAutoScroll, this, XRCID("auto_scroll"));
    Bind(wxEVT_MENU, &ChatAIWindow::OnHistory, this, XRCID("prompt_history"));

    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnSendUI, this, wxID_EXECUTE);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnStopUI, this, wxID_STOP);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnBusyUI, this, wxID_CLEAR);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnBusyUI, this, wxID_REFRESH);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnSendUI, this, wxID_SETUP);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnAutoScrollUI, this, XRCID("auto_scroll"));
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnHistoryUI, this, XRCID("prompt_history"));
    m_activeModel->Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnBusyUI, this);

    m_stcInput->CmdKeyClear('R', wxSTC_KEYMOD_CTRL);

    m_markdownStyler = std::make_unique<MarkdownStyler>(m_stcOutput);
    m_markdownStyler->Bind(wxEVT_MARKDOWN_LINK_CLICKED, [](clCommandEvent& event) {
        wxString url = event.GetString();
        ::wxLaunchDefaultBrowser(url);
    });

    m_statusPanel = new IndicatorPanel(this);
    GetSizer()->Add(m_statusPanel, wxSizerFlags(0).Expand());

    CallAfter(&ChatAIWindow::LoadGlobalConfig);
    CallAfter(&ChatAIWindow::PopulateModels);
    CallAfter(&ChatAIWindow::RestoreUI);
    ShowIndicator(false);

    m_cancel_token = std::make_shared<llm::CancellationToken>();
}

ChatAIWindow::~ChatAIWindow()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &ChatAIWindow::OnUpdateTheme, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &ChatAIWindow::OnFileSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &ChatAIWindow::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &ChatAIWindow::OnWorkspaceClosed, this);

    /// LLM events
    Unbind(wxEVT_LLM_CHAT_STARTED, &ChatAIWindow::OnChatStarted, this);
    Unbind(wxEVT_LLM_OUTPUT, &ChatAIWindow::OnChatAIOutput, this);
    Unbind(wxEVT_LLM_OUTPUT_DONE, &ChatAIWindow::OnChatAIOutputDone, this);
    Unbind(wxEVT_LLM_THINK_SATRTED, &ChatAIWindow::OnThinkingStart, this);
    Unbind(wxEVT_LLM_THINK_ENDED, &ChatAIWindow::OnThinkingEnd, this);
    Unbind(wxEVT_LLM_MODELS_LOADED, &ChatAIWindow::OnModelsLoaded, this);
    clConfig::Get().Write("chat-ai/sash-position", m_mainSplitter->GetSashPosition());
}

wxString ChatAIWindow::GetConfigurationFilePath() const
{
    WriteOptions opts{.converter = nullptr, .force_global = true};
    return FileManager::GetSettingFileFullPath(kAssistantConfigFile, opts);
}

void ChatAIWindow::OnSend(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSendPrompt();
}

void ChatAIWindow::DoSendPrompt()
{
    wxBusyCursor bc{};

    wxString active_model = m_activeModel->GetStringSelection();
    if (active_model.empty()) {
        ::wxMessageBox(_("Please choose a model first."), "CodeLite", wxICON_WARNING | wxOK | wxCENTRE);
        return;
    }

    wxString prompt = m_stcInput->GetText();
    prompt.Trim().Trim(false);
    m_cancel_token->Reset();
    llm::Manager::GetInstance().Chat(
        this, prompt, m_cancel_token, llm::ChatOptions::kDefault, m_activeModel->GetStringSelection());

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

void ChatAIWindow::OnSendUI(wxUpdateUIEvent& event)
{
    wxString prompt = m_stcInput->GetText();
    prompt.Trim().Trim(false);
    event.Enable(!llm::Manager::GetInstance().IsBusy() && !prompt.empty() &&
                 !m_activeModel->GetStringSelection().empty());
}

void ChatAIWindow::OnModelChanged(wxCommandEvent& event)
{
    llm::Manager::GetInstance().GetConfig().SetSelectedModelName(m_activeModel->GetStringSelection());
    llm::Manager::GetInstance().GetConfig().Save();
    DoReset();
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
        bg_colour = bg_colour.ChangeLightness(95);
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
    WriteOptions opts{.force_global = true};
    wxString global_config_path = FileManager::GetSettingFileFullPath(kAssistantConfigFile, opts);
    if (!wxFileName::Exists(global_config_path)) {
        // create it
        if (!FileUtils::WriteFileContent(global_config_path, kDefaultSettings)) {
            ::wxMessageBox(wxString() << _("Failed to create configuration file:\n") << global_config_path,
                           "CodeLite",
                           wxICON_WARNING | wxOK | wxCENTER);
            return;
        }
    }
    clGetManager()->OpenFile(global_config_path);
}

void ChatAIWindow::OnRefreshModelList(wxCommandEvent& event)
{
    wxUnusedVar(event);
    PopulateModels();
}

void ChatAIWindow::DoReset()
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
}

void ChatAIWindow::OnNewSession(wxCommandEvent& event)
{
    wxUnusedVar(event);
    m_cancel_token->Cancel();
    DoReset();
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

void ChatAIWindow::OnFileSaved(clCommandEvent& event)
{
    // Always call Skip()
    event.Skip();
    CHECK_PTR_RET(clGetManager()->GetActiveEditor());

    wxString filepath = clGetManager()->GetActiveEditor()->GetRemotePathOrLocal();
    if (filepath != GetConfigurationFilePath()) {
        return;
    }

    // Reload configuration
    wxBusyCursor bc{};
    if (llm::Manager::GetInstance().ReloadConfig(clGetManager()->GetActiveEditor()->GetEditorText())) {
        clGetManager()->SetStatusMessage(_("ChatAI configuration re-loaded successfully"), 3);
        // Refresh the model list.
        PopulateModels();
    }
}

void ChatAIWindow::OnInputUI(wxUpdateUIEvent& event) { event.Enable(true); }

void ChatAIWindow::OnModelsLoaded(clLLMEvent& event)
{
    wxArrayString models = event.GetStrings();
    m_activeModel->Clear();
    m_activeModel->Append(models);

    auto active_model = llm::Manager::GetInstance().GetConfig().GetSelectedModel();
    if (!models.empty()) {
        if (!active_model.empty()) {
            int where = m_activeModel->FindString(active_model);
            m_activeModel->SetSelection(where == wxNOT_FOUND ? 0 : where);

        } else {
            m_activeModel->SetSelection(0);
            active_model = m_activeModel->GetString(0);
        }
    }
    llm::Manager::GetInstance().SetActiveModel(active_model);
}

void ChatAIWindow::PopulateModels() { llm::Manager::GetInstance().LoadModels(this); }

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

void ChatAIWindow::OnWorkspaceLoaded(clWorkspaceEvent& event)
{
    event.Skip();
    WriteOptions opts{.converter = nullptr, .force_global = true};
    auto content = FileManager::ReadSettingsFileContent(kAssistantConfigFile, opts).value_or(kDefaultSettings);
    llm::Manager::GetInstance().ReloadConfig(content, false);
}

void ChatAIWindow::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    DoReset();
    CallAfter(&ChatAIWindow::LoadGlobalConfig);
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
    llm::Manager::GetInstance().ReloadConfig(GetGlobalSettings().value_or(kDefaultSettings), false);
}

void ChatAIWindow::OnAutoScroll(wxCommandEvent& event) { m_autoScroll = event.IsChecked(); }

void ChatAIWindow::OnAutoScrollUI(wxUpdateUIEvent& event) { event.Check(m_autoScroll); }

void ChatAIWindow::OnBusyUI(wxUpdateUIEvent& event)
{
    event.Enable(llm::Manager::GetInstance().IsAvailable() && !llm::Manager::GetInstance().IsBusy());
}

void ChatAIWindow::OnHistory(wxCommandEvent& event)
{
    const auto& config = llm::Manager::GetInstance().GetConfig();
    clSingleChoiceDialog dlg(EventNotifier::Get()->TopFrame(), config.GetHistory());
    dlg.SetLabel(_("Chat history"));
    if (dlg.ShowModal() != wxID_OK) {
        return;
    }

    // Update the prompt field.
    m_stcInput->SetText(dlg.GetSelection());
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
