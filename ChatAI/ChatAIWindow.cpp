#include "ChatAIWindow.hpp"

#include "ChatAI.hpp"
#include "ColoursAndFontsManager.h"
#include "FileManager.hpp"
#include "MarkdownStyler.hpp"
#include "OllamaClient.hpp"
#include "StringUtils.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
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
const wxString kDefaultSettings =
    R"#({
  "history_size": 50,
  "server_url": "http://127.0.0.1:11434",
  "servers": {},
  "http_headers": {
    "Host": "127.0.0.1"
  },
  "models": {
    "default": {
      "options": {
        "num_ctx": 32768,
        "temperature": 0
      },
      "think_end_tag": "</think>",
      "think_start_tag": "</think>"
    }
  }
})#";

const wxString CHAT_AI_LABEL = _("Chat AI");
const wxString LONG_MODEL_NAME = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
constexpr const char* kAssistantConfigFile = "assistant.json";

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

ChatAIWindow::ChatAIWindow(wxWindow* parent, ChatAI* plugin)
    : AssistanceAIChatWindowBase(parent)
    , m_plugin(plugin)
{
    auto images = m_toolbar->GetBitmapsCreateIfNeeded();
    m_toolbar->AddTool(wxID_CLEAR, _("Clear everything and start a new session"), images->Add("clear"));
    m_toolbar->AddSeparator();
    m_activeModel = new wxChoice(
        m_toolbar, wxID_ANY, wxDefaultPosition, wxSize(GetTextExtent(LONG_MODEL_NAME).GetWidth(), wxNOT_FOUND));
    m_activeModel->SetToolTip(_("Change model. Changing a model will also clear your chat history"));
    m_toolbar->AddControl(m_activeModel);
    m_toolbar->AddTool(wxID_REFRESH, _("Load models list"), images->Add("debugger_restart"));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(wxID_SETUP, _("Settings"), images->Add("cog"));
    m_toolbar->Realize();

    PopulateModels();
    m_activeModel->Bind(wxEVT_CHOICE, &ChatAIWindow::OnModelChanged, this);

    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &ChatAIWindow::OnUpdateTheme, this);
    EventNotifier::Get()->Bind(wxEVT_OLLAMA_OUTPUT, &ChatAIWindow::OnChatAIOutput, this);
    EventNotifier::Get()->Bind(wxEVT_OLLAMA_CHAT_DONE, &ChatAIWindow::OnChatAIOutputDone, this);
    EventNotifier::Get()->Bind(wxEVT_OLLAMA_LIST_MODELS, &ChatAIWindow::OnModels, this);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &ChatAIWindow::OnFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &ChatAIWindow::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &ChatAIWindow::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_OLLAMA_LOG, &ChatAIWindow::OnLog, this);

    m_stcInput->Bind(wxEVT_KEY_DOWN, &ChatAIWindow::OnKeyDown, this);
    m_stcOutput->Bind(wxEVT_KEY_DOWN, &ChatAIWindow::OnKeyDown, this);
    m_stcOutput->SetReadOnly(true);

    Bind(wxEVT_MENU, &ChatAIWindow::OnNewSession, this, wxID_CLEAR);
    Bind(wxEVT_MENU, &ChatAIWindow::OnRefreshModelList, this, wxID_REFRESH);
    Bind(wxEVT_MENU, &ChatAIWindow::OnSettings, this, wxID_SETUP);

    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnSendUI, this, wxID_CLEAR);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnSendUI, this, wxID_REFRESH);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnSendUI, this, wxID_SETUP);
    m_activeModel->Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnSendUI, this);

    m_stcInput->CmdKeyClear('R', wxSTC_KEYMOD_CTRL);

    m_logView = new wxTerminalOutputCtrl(m_panelLog);
    m_logView->SetSink(this);
    m_panelLog->GetSizer()->Add(m_logView, wxSizerFlags(1).Expand());

    m_plugin->GetClient().SetLogSink([](ollama::LogLevel level, std::string message) {
        // For now, just print it to the log.
        OllamaEvent log_event{wxEVT_OLLAMA_LOG};
        log_event.SetLogLevel(level);
        log_event.SetStringRaw(message);
        EventNotifier::Get()->AddPendingEvent(log_event);
    });
    m_markdownStyler = std::make_unique<MarkdownStyler>(m_stcOutput);
    ShowIndicator(false);
    CallAfter(&ChatAIWindow::LoadGlobalConfig);
}

ChatAIWindow::~ChatAIWindow()
{
    EventNotifier::Get()->Unbind(wxEVT_CL_THEME_CHANGED, &ChatAIWindow::OnUpdateTheme, this);
    EventNotifier::Get()->Unbind(wxEVT_OLLAMA_OUTPUT, &ChatAIWindow::OnChatAIOutput, this);
    EventNotifier::Get()->Unbind(wxEVT_OLLAMA_CHAT_DONE, &ChatAIWindow::OnChatAIOutputDone, this);
    EventNotifier::Get()->Unbind(wxEVT_OLLAMA_LIST_MODELS, &ChatAIWindow::OnModels, this);
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &ChatAIWindow::OnFileSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_LOADED, &ChatAIWindow::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &ChatAIWindow::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_OLLAMA_LOG, &ChatAIWindow::OnLog, this);
}

wxString ChatAIWindow::GetConfigurationFilePath() const
{
    return FileManager::GetSettingFileFullPath(kAssistantConfigFile);
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

    m_plugin->GetClient().Send(m_stcInput->GetText(), m_activeModel->GetStringSelection());
    m_stcInput->ClearAll();
    ShowIndicator(true);
}

void ChatAIWindow::OnSendUI(wxUpdateUIEvent& event) { event.Enable(!m_plugin->GetClient().IsBusy()); }

void ChatAIWindow::OnModelChanged(wxCommandEvent& event)
{
    m_plugin->GetConfig().SetSelectedModelName(m_activeModel->GetStringSelection());
    DoReset();
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

void ChatAIWindow::DoLogMessage(const wxString& message, ollama::LogLevel log_level)
{
    clAnsiEscapeCodeColourBuilder builder;
    switch (log_level) {
    case ollama::LogLevel::kError:
        builder.Add(message, AnsiColours::Red(), true);
        break;
    case ollama::LogLevel::kWarning:
        builder.Add(message, AnsiColours::Yellow(), true);
        break;
    case ollama::LogLevel::kTrace:
    case ollama::LogLevel::kDebug:
        builder.Add(message, AnsiColours::Gray(), true);
        break;
    case ollama::LogLevel::kInfo:
    default:
        builder.Add(message, AnsiColours::Green());
        break;
    }
    wxString line = builder.GetString();
    line.Trim() << "\n";

    wxStringView sv{line.c_str(), line.length()};
    m_logView->StyleAndAppend(sv, nullptr);
}

void ChatAIWindow::OnLog(OllamaEvent& event)
{
    wxString content = wxString::FromUTF8(event.GetStringRaw());
    DoLogMessage(content, event.GetLogLevel());
}

void ChatAIWindow::OnSettings(wxCommandEvent& event)
{
    wxUnusedVar(event);
    /// Create the workspace settings file (do nothing if the file already exists).
    DoCreateWorkspaceSettings();

    if (clWorkspaceManager::Get().IsWorkspaceOpened()) {
        auto editor = clWorkspaceManager::Get().GetWorkspace()->CreateOrOpenSettingFile(kAssistantConfigFile);
        if (!editor) {
            ::wxMessageBox(wxString() << _("Could not open file: ") << kAssistantConfigFile,
                           "CodeLite",
                           wxICON_ERROR | wxOK | wxCENTER);
            return;
        }

        if (!editor->GetEditorText().empty()) {
            return;
        }

        // Place some default content, if we have a global configuration - copy the settings from that file.
        wxString default_content = GetGlobalSettings().value_or(kDefaultSettings);
        editor->SetEditorText(default_content);
    } else {
        // Global settings
        wxFileName global_config{GetConfigurationFilePath()};
        if (!global_config.FileExists()) {
            global_config.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            if (!FileUtils::WriteFileContent(global_config, kDefaultSettings)) {
                ::wxMessageBox(wxString() << _("Failed to create configuration file:\n") << global_config.GetFullPath(),
                               "CodeLite",
                               wxICON_WARNING | wxOK | wxCENTER);
                return;
            }
        }
        clGetManager()->OpenFile(global_config.GetFullPath());
    }
}

void ChatAIWindow::OnRefreshModelList(wxCommandEvent& event)
{
    wxUnusedVar(event);
    PopulateModels();
}

void ChatAIWindow::DoReset()
{
    DoClearOutputView();
    m_stcInput->ClearAll();
    m_plugin->GetClient().Clear();
    ShowIndicator(false);
    m_thinking = false;
}

void ChatAIWindow::DoClearOutputView()
{
    m_stcOutput->SetReadOnly(false);
    m_stcOutput->ClearAll();
    m_stcOutput->SetReadOnly(true);
    m_markdownStyler = std::make_unique<MarkdownStyler>(m_stcOutput);
}

void ChatAIWindow::OnNewSession(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoReset();
}

void ChatAIWindow::OnChatAIOutput(OllamaEvent& event)
{
    bool changed_state = (event.IsThinking() != m_thinking);
    if (changed_state) {
        if (event.IsThinking()) {
            // we just started thinking...
            AppendOutput("**Thinking** ...");
        } else {
            AppendOutput(" done");
        }
    }

    m_thinking = event.IsThinking();

    wxString content = wxString::FromUTF8(event.GetStringRaw());
    switch (event.GetReason()) {
    case ollama::Reason::kFatalError:
        ::wxMessageBox(content, "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
        DoReset();
        m_thinking = false;
        return;
    case ollama::Reason::kLogNotice:
    case ollama::Reason::kLogDebug:
        clDEBUG() << content << endl;
        break;
    case ollama::Reason::kDone:
        m_thinking = false;
        break;
    case ollama::Reason::kPartialResult:
        if (!event.IsThinking()) {
            AppendOutput(content);
        }
        break;
    }
}

void ChatAIWindow::OnChatAIOutputDone(OllamaEvent& event)
{
    AppendOutput(wxT("\n---\n"));
    StyleOutput();
    m_thinking = false;

    // Move the focus back to the input text control
    m_stcInput->CallAfter(&wxStyledTextCtrl::SetFocus);
    ShowIndicator(false);
}

void ChatAIWindow::ShowIndicator(bool show)
{
    if (show) {
        m_activityIndicator->Show();
        m_activityIndicator->Start();
    } else {
        m_activityIndicator->Stop();
        m_activityIndicator->Hide();
    }
    m_activityIndicator->GetParent()->SendSizeEvent();
}

void ChatAIWindow::OnFileSaved(clCommandEvent& event)
{
    // Always call Skip()
    event.Skip();
    CHECK_PTR_RET(clGetManager()->GetActiveEditor());

    wxString filepath = clGetManager()->GetActiveEditor()->GetRemotePathOrLocal();
    if (filepath == GetConfigurationFilePath()) {
        // Reload configuration
        wxBusyCursor bc{};
        m_plugin->GetClient().ReloadConfig(clGetManager()->GetActiveEditor()->GetEditorText());
        clGetManager()->SetStatusMessage(_("ChatAI configuration re-loaded successfully"), 3);
    }
}

void ChatAIWindow::OnModels(OllamaEvent& event)
{
    m_activeModel->Clear();
    for (const auto& model : event.GetModels()) {
        m_activeModel->Append(model);
    }

    const auto& models = event.GetModels();
    const auto& active_model = m_plugin->GetConfig().GetSelectedModel();

    if (!models.empty()) {
        if (!active_model.empty()) {
            int where = m_activeModel->FindString(active_model);
            m_activeModel->SetSelection(where == wxNOT_FOUND ? 0 : where);

        } else {
            m_activeModel->SetSelection(0);
        }
    }
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

void ChatAIWindow::AppendOutput(const wxString& text)
{
    CHECK_COND_RET(!text.empty());
    m_stcOutput->SetReadOnly(false);
    m_stcOutput->AppendText(text);
    m_stcOutput->SetReadOnly(true);

    bool scroll_to_end{true};
    if (wxWindow::FindFocus() == m_stcOutput) {
        scroll_to_end = false;
    }

    if (scroll_to_end) {
        m_stcOutput->ScrollToEnd();
    }
    clSTCHelper::SetCaretAt(m_stcOutput, m_stcOutput->GetLastPosition());
}

void ChatAIWindow::StyleOutput()
{
    m_markdownStyler->StyleText();

    bool scroll_to_end{true};
    if (wxWindow::FindFocus() == m_stcOutput) {
        scroll_to_end = false;
    }

    if (scroll_to_end) {
        m_stcOutput->ScrollToEnd();
    }
    clSTCHelper::SetCaretAt(m_stcOutput, m_stcOutput->GetLastPosition());
}

void ChatAIWindow::OnWorkspaceLoaded(clWorkspaceEvent& event)
{
    event.Skip();
    DoCreateWorkspaceSettings();
    auto content = FileManager::ReadSettingsFileContent(kAssistantConfigFile).value_or(kDefaultSettings);
    m_plugin->GetClient().ReloadConfig(content);
}

void ChatAIWindow::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    DoReset();
    CallAfter(&ChatAIWindow::LoadGlobalConfig);
}

bool ChatAIWindow::DoCreateWorkspaceSettings()
{
    wxString config_path = FileManager::GetSettingFileFullPath(kAssistantConfigFile);

    clDEBUG() << "Checking if file:" << config_path << "exists..." << endl;
    if (FileManager::FileExists(config_path)) {
        clDEBUG() << "Yes." << endl;
        return true;
    }
    clDEBUG() << "No." << endl;

    // New file, create the workspace file content.
    wxString default_content = kDefaultSettings;
    if (clWorkspaceManager::Get().IsWorkspaceOpened()) {
        // Try to copy the content from the global settings.
        default_content = GetGlobalSettings().value_or(kDefaultSettings);
    }
    return FileManager::WriteSettingsFileContent(kAssistantConfigFile, default_content);
}

void ChatAIWindow::LoadGlobalConfig()
{
    wxBusyCursor bc{};
    m_plugin->GetClient().ReloadConfig(GetGlobalSettings().value_or(kDefaultSettings));
}

void ChatAIWindow::OnStop(wxCommandEvent& event)
{
    event.Skip();
    m_plugin->GetClient().Interrupt();
}

void ChatAIWindow::OnStopUI(wxUpdateUIEvent& event) { event.Enable(m_plugin->GetClient().IsBusy()); }
