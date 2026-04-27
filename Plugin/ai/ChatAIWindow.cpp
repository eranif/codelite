#include "ChatAIWindow.hpp"

#include "ChatAIWindowFrame.hpp"
#include "ChatHistoryDialog.hpp"
#include "ColoursAndFontsManager.h"
#include "MarkdownStyler.hpp"
#if USE_SFTP
#include "SFTPBrowserDlg.h"
#include "clSFTPManager.hpp"
#endif
#include "ai/EndpointModelSelector.hpp"
#include "ai/LLMManager.hpp"
#include "aui/clAuiToolBarArt.h"
#include "clAnsiEscapeCodeColourBuilder.hpp"
#include "clSTCHelper.hpp"
#include "cl_aui_tool_stickness.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "fileutils.h"
#include "globals.h"
#include "wx/wupdlock.h"
#include "wxCustomControls.hpp"

#include <algorithm>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>

namespace
{
const wxString LONG_MODEL_NAME = "claude-sonnet-4-5-1234567890";

enum StatusBarIndex {
    kProgressText,
    kCost,
    kUsage,
    kProgress,
    kLast,
};

} // namespace

ChatAIWindow::ChatAIWindow(wxWindow* parent)
    : AssistanceAIChatWindowBase(parent)
{
    auto images = clGetManager()->GetStdIcons();
    m_gaugeContextUsed->SetSizeHints(-1, 10);
    m_inputEditHelper = std::make_unique<clEditEventsHandler>(m_stcInput);
    m_outputEditHelper = std::make_unique<clEditEventsHandler>(m_stcOutput);

    m_stcInput->SetToolTip(_("Hit Ctrl-ENTER to submit.\nType / to see list of commands"));

    clAuiToolBarArt::AddTool(m_toolbar, wxID_OPEN, _("Load Chat"), images->LoadBitmap("file_open"));
    clAuiToolBarArt::AddTool(m_toolbar, wxID_SAVE, _("Save Chat"), images->LoadBitmap("file_save"));
    m_toolbar->AddSeparator();
    m_model_selector = std::make_unique<EndpointModelSelector>(m_toolbar);
    m_model_selector->SetOnEndpointChangedCallback([this]() {
        DoClearOutputView();
        UpdateStatusBar();
    });

    clAuiToolBarArt::AddTool(m_toolbar, wxID_EXECUTE, _("Submit"), images->LoadBitmap("fold"));
    clAuiToolBarArt::AddTool(m_toolbar, wxID_STOP, _("Stop"), images->LoadBitmap("execute_stop"));
    m_toolbar->AddSeparator();
    clAuiToolBarArt::AddTool(m_toolbar, wxID_REFRESH, _("Restart the client"), images->LoadBitmap("debugger_restart"));
    m_toolbar->AddSeparator();
    clAuiToolBarArt::AddTool(
        m_toolbar, XRCID("wxID_SETTINGS"), _("Options"), images->LoadBitmap("cog"), wxEmptyString, wxITEM_DROPDOWN);

    clAuiToolBarArt::AddTool(m_toolbar,
                             XRCID("auto_scroll"),
                             _("Enable auto scrolling"),
                             images->LoadBitmap("link_editor"),
                             wxEmptyString,
                             wxITEM_CHECK);
    m_toolbar->AddSeparator();

    clAuiToolBarArt::AddTool(m_toolbar, wxID_CLEAR, _("Clear the chat history"), images->LoadBitmap("clear"));

    clAuiToolBarArt::Finalise(m_toolbar);
    m_toolbar->Realize();

    m_toolbar->Bind(wxEVT_AUITOOLBAR_TOOL_DROPDOWN, &ChatAIWindow::OnOptions, this, XRCID("wxID_SETTINGS"));

    EventNotifier::Get()->Bind(wxEVT_CL_THEME_CHANGED, &ChatAIWindow::OnUpdateTheme, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_LOADED, &ChatAIWindow::OnWorkspaceLoaded, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &ChatAIWindow::OnWorkspaceClosed, this);

    /// LLM events
    Bind(wxEVT_LLM_CHAT_STARTED, &ChatAIWindow::OnChatStarted, this);
    Bind(wxEVT_LLM_OUTPUT, &ChatAIWindow::OnChatAIOutput, this);
    Bind(wxEVT_LLM_OUTPUT_DONE, &ChatAIWindow::OnChatAIOutputDone, this);
    Bind(wxEVT_LLM_MAX_GENERATED_TOKENS, &ChatAIWindow::OnChatAIMaxGenTokens, this);

    llm::Manager::GetInstance().Bind(wxEVT_LLM_CONFIG_UPDATED, &ChatAIWindow::OnLLMConfigUpdate, this);
    llm::Manager::GetInstance().Bind(wxEVT_LLM_STARTED, &ChatAIWindow::OnLLMConfigUpdate, this);
    llm::Manager::GetInstance().Bind(wxEVT_LLM_WORKER_IDLE, &ChatAIWindow::OnLLMWorkerIdle, this);
    llm::Manager::GetInstance().Bind(wxEVT_LLM_WORKER_BUSY, &ChatAIWindow::OnLLMWorkerBusy, this);
    m_stcInput->Bind(wxEVT_KEY_DOWN, &ChatAIWindow::OnKeyDown, this);
    m_stcOutput->Bind(wxEVT_KEY_DOWN, &ChatAIWindow::OnKeyDown, this);
    m_stcOutput->SetCodePage(wxSTC_CP_UTF8);
    m_stcInput->SetCodePage(wxSTC_CP_UTF8);
    m_stcOutput->SetReadOnly(true);

    m_commandDispatchTable.emplace("/clear", [this]() { CallAfter(&ChatAIWindow::DoCommandClear); });
    m_commandDispatchTable.emplace("/context", [this]() { CallAfter(&ChatAIWindow::DoCommandContext); });
    m_commandDispatchTable.emplace("/save", [this]() { CallAfter(&ChatAIWindow::DoCommandSave); });

    Bind(wxEVT_MENU, &ChatAIWindow::OnSaveSession, this, wxID_SAVE);
    Bind(wxEVT_MENU, &ChatAIWindow::OnLoadSession, this, wxID_OPEN);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnSaveSessionUI, this, wxID_SAVE);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnLoadSessionUI, this, wxID_OPEN);
    Bind(wxEVT_MENU, &ChatAIWindow::OnClearSession, this, wxID_CLEAR);
    Bind(wxEVT_MENU, &ChatAIWindow::OnRestartClient, this, wxID_REFRESH);
    Bind(wxEVT_MENU, &ChatAIWindow::OnSend, this, wxID_EXECUTE);
    Bind(wxEVT_MENU, &ChatAIWindow::OnStop, this, wxID_STOP);
    Bind(wxEVT_MENU, &ChatAIWindow::OnAutoScroll, this, XRCID("auto_scroll"));
    Bind(wxEVT_MENU, &ChatAIWindow::OnDetachView, this, XRCID("detach_view"));

    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnDetachViewUI, this, XRCID("detach_view"));
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnSendUI, this, wxID_EXECUTE);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnStopUI, this, wxID_STOP);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnClearOutputViewUI, this, wxID_CLEAR);
    Bind(wxEVT_UPDATE_UI, &ChatAIWindow::OnAutoScrollUI, this, XRCID("auto_scroll"));

    m_stcInput->CmdKeyClear('R', wxSTC_KEYMOD_CTRL);
    m_stcInput->Bind(wxEVT_STC_CHARADDED, &ChatAIWindow::OnCharAdded, this);
    m_stcInput->Bind(wxEVT_STC_AUTOCOMP_SELECTION, &ChatAIWindow::OnSelection, this);
    Bind(wxEVT_SIZE, &ChatAIWindow::OnSize, this);

    m_markdownStyler = std::make_unique<MarkdownStyler>(m_stcOutput);
    m_markdownStyler->Bind(wxEVT_MARKDOWN_LINK_CLICKED, [](clCommandEvent& event) {
        wxString url = event.GetString();
        ::wxLaunchDefaultBrowser(url);
    });

    m_statusBar = new wxStatusBar(this, wxID_ANY, (wxSTB_DEFAULT_STYLE & ~wxSTB_SIZEGRIP));
    m_statusBar->SetFieldsCount(4);
    GetSizer()->Add(m_statusBar, wxSizerFlags(0).Expand());

    auto indicator_size = FromDIP(wxSize(16, 16));

#if wxCHECK_VERSION(3, 3, 0)
    m_activityIndicator = new wxActivityIndicator(m_statusBar, wxID_ANY, wxDefaultPosition, indicator_size);
    m_statusBar->AddFieldControl(StatusBarIndex::kProgress, m_activityIndicator);
#endif

    int widths[] = {-1, -3, -3, indicator_size.GetWidth()};
    int styles[] = {wxSB_FLAT, wxSB_FLAT, wxSB_FLAT, wxSB_FLAT};
    m_statusBar->SetStatusWidths(4, widths);
    m_statusBar->SetStatusStyles(4, styles);
    CallAfter(&ChatAIWindow::RestoreUI);
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
    Unbind(wxEVT_LLM_MAX_GENERATED_TOKENS, &ChatAIWindow::OnChatAIMaxGenTokens, this);

    llm::Manager::GetInstance().Unbind(wxEVT_LLM_CONFIG_UPDATED, &ChatAIWindow::OnLLMConfigUpdate, this);
    llm::Manager::GetInstance().Unbind(wxEVT_LLM_STARTED, &ChatAIWindow::OnLLMConfigUpdate, this);
    llm::Manager::GetInstance().Unbind(wxEVT_LLM_WORKER_IDLE, &ChatAIWindow::OnLLMWorkerIdle, this);
    llm::Manager::GetInstance().Unbind(wxEVT_LLM_WORKER_BUSY, &ChatAIWindow::OnLLMWorkerBusy, this);

    Unbind(wxEVT_SIZE, &ChatAIWindow::OnSize, this);

    clConfig::Get().Write("chat-ai/sash-position", m_mainSplitter->GetSashPosition());
}

namespace
{
void SetCachePolicy(llm::CachePolicy policy)
{
    wxMessageBox("Setting cache policy: " + std::to_string((int)policy));
    auto& llm_manager = llm::Manager::GetInstance();
    auto& conf = llm_manager.GetConfig();
    conf.SetCachePolicy(policy);
    llm_manager.SetCachingPolicy(policy);
    conf.Save(false);
}
} // namespace

void ChatAIWindow::OnOptions(wxAuiToolBarEvent& event)
{
    auto& llm_manager = llm::Manager::GetInstance();

    bool is_docked = clConfig::Get().Read(CHAT_AI_DOCKED, true);
    wxMenu menu;
    wxMenu* caching_policy_menu = new wxMenu;
    auto& conf = llm_manager.GetConfig();
    menu.Append(XRCID("wxID_TOOLS_ENABLED"), _("Enable MCP Tools"), wxEmptyString, wxITEM_CHECK)
        ->Check(conf.AreToolsEnabled());
    menu.Bind(
        wxEVT_MENU,
        [&conf](wxCommandEvent& e) {
            conf.SetToolsEnabled(e.IsChecked());
            conf.Save(false);
        },
        XRCID("wxID_TOOLS_ENABLED"));

    caching_policy_menu->Append(XRCID("wxID_CACHING_POLICY_NONE"), _("None"), wxEmptyString, wxITEM_CHECK)
        ->Check(llm_manager.GetCachingPolicy() == llm::CachePolicy::kNone);
    caching_policy_menu->Append(XRCID("wxID_CACHING_POLICY_AUTO"), _("Auto"), wxEmptyString, wxITEM_CHECK)
        ->Check(llm_manager.GetCachingPolicy() == llm::CachePolicy::kAuto);
    caching_policy_menu
        ->Append(XRCID("wxID_CACHING_POLICY_STATIC_CONTENT"), _("Static Content"), wxEmptyString, wxITEM_CHECK)
        ->Check(llm_manager.GetCachingPolicy() == llm::CachePolicy::kStatic);

    menu.Bind(
        wxEVT_MENU,
        [](wxCommandEvent& e) {
            wxUnusedVar(e);
            SetCachePolicy(llm::CachePolicy::kNone);
        },
        XRCID("wxID_CACHING_POLICY_NONE"));
    menu.Bind(
        wxEVT_MENU,
        [](wxCommandEvent& e) {
            wxUnusedVar(e);
            SetCachePolicy(llm::CachePolicy::kAuto);
        },
        XRCID("wxID_CACHING_POLICY_AUTO"));
    menu.Bind(
        wxEVT_MENU,
        [](wxCommandEvent& e) {
            wxUnusedVar(e);
            SetCachePolicy(llm::CachePolicy::kStatic);
        },
        XRCID("wxID_CACHING_POLICY_STATIC_CONTENT"));
    menu.Append(wxID_ANY, _("Prompt Caching"), caching_policy_menu);
    auto prompts = llm_manager.GetConfig().GetAllPrompts();
    if (!prompts.empty()) {
        wxMenu* prompt_selection = new wxMenu;
        for (const auto& [label, prompt] : prompts) {
            auto item_id = wxXmlResource::GetXRCID(label);
            prompt_selection->Append(item_id, label);
            prompt_selection->Bind(
                wxEVT_MENU,
                [prompt, this](wxCommandEvent& event) {
                    wxUnusedVar(event);
                    m_stcInput->SetText(prompt);
                    m_stcInput->CallAfter(&wxStyledTextCtrl::SetFocus);
                },
                item_id);
        }
        menu.Append(wxID_ANY, _("Insert Prompt"), prompt_selection);
    }
    menu.AppendSeparator();
    menu.Append(XRCID("dock-chat-window"), _("Dock Chat Window"), wxEmptyString, wxITEM_CHECK)->Check(is_docked);
    menu.Bind(
        wxEVT_MENU,
        [](wxCommandEvent& event) {
            clConfig::Get().Write(CHAT_AI_DOCKED, event.IsChecked());
            ::clMessageBox(_("A restart is required for changes to take effect"),
                           "CodeLite",
                           wxICON_INFORMATION | wxOK | wxCENTER);
        },
        XRCID("dock-chat-window"));

    menu.AppendSeparator();
    menu.Append(XRCID("chat_history"), _("Manage Chat History"))->Enable(CurrentEndpointHasHistory());
    menu.Bind(wxEVT_MENU, &ChatAIWindow::OnHistory, this, XRCID("chat_history"));

    clAuiToolStickness stickness{m_toolbar, event.GetId()};
    // line up our menu with the button
    wxRect rect = m_toolbar->GetToolRect(event.GetId());
    wxPoint pt = m_toolbar->ClientToScreen(rect.GetBottomLeft());
    pt = ScreenToClient(pt);
    PopupMenu(&menu, pt);
}

void ChatAIWindow::OnSend(wxCommandEvent& event)
{
    wxUnusedVar(event);
    DoSendPrompt();
}

void ChatAIWindow::DoSendPrompt()
{
    wxBusyCursor bc{};

    if (!m_model_selector->IsOk()) {
        ::clMessageBox(_("Please choose an endpoint/model."), "CodeLite", wxICON_WARNING | wxOK | wxCENTRE);
        return;
    }

    wxString prompt = m_stcInput->GetText();
    prompt.Trim().Trim(false);
    m_cancel_token->Reset();

    if (prompt.empty()) {
        return;
    }

    // Always clear the prompt window.
    m_stcInput->ClearAll();

    // Check if someone is waiting for this message
    auto promise = PopPromise();
    if (promise.has_value()) {
        promise.value()->set_value(prompt.ToStdString(wxConvUTF8));
        return;
    }

    llm::ChatOptions chat_options{llm::ChatOptions::kDefault};
    if (!llm::Manager::GetInstance().GetConfig().AreToolsEnabled()) {
        // No tools.
        llm::AddFlagSet(chat_options, llm::ChatOptions::kNoTools);
    }
    llm::Manager::GetInstance().Chat(this, prompt, m_cancel_token, chat_options);

    wxString text_to_append;
    text_to_append << wxT("❰") << wxGetUserId() << wxT("❱\n") << prompt << "\n\n";

    int last_char = m_stcOutput->GetCharAt(m_stcOutput->GetLastPosition());
    int before_last_char = m_stcOutput->GetCharAt(m_stcOutput->GetLastPosition() - 1);

    wxString prefix = "";
    if (!m_stcOutput->IsEmpty()) {
        if (last_char == '\n' && before_last_char != '\n') {
            prefix << "\n";
        } else if (last_char != '\n') {
            prefix << "\n\n";
        }
    }
    AppendTextWithLF(prefix + text_to_append);
    m_stcInput->ClearAll();
}

void ChatAIWindow::OnClearOutputViewUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_stcOutput->IsEmpty() && !llm::Manager::GetInstance().IsBusy());
}

void ChatAIWindow::OnSendUI(wxUpdateUIEvent& event)
{
    wxString prompt = m_stcInput->GetText();
    prompt.Trim().Trim(false);
    event.Enable((m_state == ChatState::kReady) && !prompt.empty() && m_model_selector->IsOk());
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
    if (::clMessageBox(_("Restarting the client will result in the loss of all the current session data.\nDo you wish "
                         "to continue?"),
                       "CodeLite",
                       wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxICON_WARNING) != wxYES) {
        return;
    }

    m_cancel_token->Cancel();
    DoClearOutputView();
    // Reload configuration
    wxBusyCursor bc{};
    if (llm::Manager::GetInstance().ReloadConfig(std::nullopt, false)) {
        clGetManager()->SetStatusMessage(_("LLM client successfully restarted"), 3);
        UpdateStatusBar();
    }
}

void ChatAIWindow::DoRestart()
{
    DoClearOutputView();
    llm::Manager::GetInstance().Restart();
    UpdateStatusBar();
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
    llm::Manager::GetInstance().ClearSystemMessages();
    UpdateStatusBar();
}

void ChatAIWindow::OnClearSession(wxCommandEvent& event)
{
    wxUnusedVar(event);

    wxStandardID answer = ::PromptForYesNoDialogWithCheckbox(
        _("Proceeding with this operation will result in the loss of the current chat context. You may "
          "choose to save it before continuing.\nDo you wish to proceed with clearing the context?"),
        "ChatAIWindowClearSession",
        _("Yes"),
        _("No"),
        _("Remember my answer"),
        wxYES_NO | wxICON_WARNING | wxYES_DEFAULT);

    if (answer != wxID_YES) {
        return;
    }
    DoClearOutputView();
}

void ChatAIWindow::OnChatStarted(clLLMEvent& event)
{
    event.Skip();
    m_state = ChatState::kWorking;
    AppendTextWithLF(wxT("❰assistant❱\n"));
}

void ChatAIWindow::OnThinkingStart(clLLMEvent& event)
{
    event.Skip();
    m_state = ChatState::kThinking;
}

void ChatAIWindow::OnThinkingEnd(clLLMEvent& event)
{
    event.Skip();
    m_state = ChatState::kWorking;
}

void ChatAIWindow::OnChatAIMaxGenTokens(clLLMEvent& event)
{
    wxUnusedVar(event);
    m_state = ChatState::kReady;
    m_cancel_token->Reset();
    StyleOutput();

    // Notify the user
    ::clMessageBox(wxString::FromUTF8(event.GetResponseRaw()), "CodeLite", wxOK | wxICON_WARNING);
}

void ChatAIWindow::OnChatAIOutput(clLLMEvent& event)
{
    const std::string& str = event.GetResponseRaw();
    wxString content = wxString::FromUTF8(str);
    switch (m_state) {
    case ChatState::kThinking:
        break;
    case ChatState::kWorking:
    case ChatState::kReady: {
        bool reported{false};
        if (event.GetOutputReason().has_value()) {
            if (event.GetOutputReason().value() == assistant::Reason::kToolDenied) {
                content.Trim().Trim(false).Append("\n");
                AppendTextWithLF(IconType_ToString(IconType::kError) + " " + content);
                reported = true;
            } else if (event.GetOutputReason().value() == assistant::Reason::kToolAllowed) {
                content.Trim().Trim(false).Append("\n");
                AppendTextWithLF(IconType_ToString(IconType::kSuccess) + " " + content);
                reported = true;
            }
        }

        if (!reported) {
            AppendOutput(content);
        }
    } break;
    }

    UpdateStatusBar();
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
}

void ChatAIWindow::ShowIndicator(bool show)
{
    if (show) {
#if wxCHECK_VERSION(3, 3, 0)
        m_activityIndicator->Show();
        m_activityIndicator->Start();
#endif
        m_statusBar->SetStatusText(_("Working..."), StatusBarIndex::kProgressText);
    } else {
#if wxCHECK_VERSION(3, 3, 0)
        m_activityIndicator->Stop();
        m_activityIndicator->Hide();
#endif
        m_statusBar->SetStatusText(_("Ready"), StatusBarIndex::kProgressText);
    }
}

void ChatAIWindow::UpdateStatusBar()
{
    auto& llm = llm::Manager::GetInstance();
    wxString str;
    if (llm.HasPricing()) {
        str << _("Total cost: $") << llm.GetTotalCost() << _(", Last Request cost: $") << llm.GetLastRequestCost();
    }
    m_statusBar->SetStatusText(str, StatusBarIndex::kCost);

    str.Clear();
    if (llm.GetLastRequestUsage()) {
        auto usage = llm.GetLastRequestUsage();
        str << _("Output tokens: ") << usage->output_tokens << _(", Input tokens: ") << usage->input_tokens
            << _(", Cached tokens read: ") << usage->cache_read_input_tokens;
    }
    m_statusBar->SetStatusText(str, StatusBarIndex::kUsage);
    UpdateContextWindowUsage();
}

void ChatAIWindow::OnLLMConfigUpdate(clLLMEvent& event)
{
    // Always call Skip()
    event.Skip();
    UpdateStatusBar();
}

void ChatAIWindow::OnInputUI(wxUpdateUIEvent& event) { event.Enable(true); }

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

wxString ChatAIWindow::GetText() const { return m_stcOutput->GetText(); }

void ChatAIWindow::ScrollToEnd()
{
    clSTCHelper::SetCaretAt(m_stcOutput, m_stcOutput->GetLastPosition());
    m_stcOutput->ScrollToEnd();
}

void ChatAIWindow::AppendTextWithLF(const wxString& text, bool force_style)
{
    CHECK_COND_RET(!text.empty());

    // Make sure our text appear on its own line
    wxString current_text = m_stcOutput->GetText();
    if (!current_text.empty() && !current_text.EndsWith("\n") && !text.StartsWith("\n")) {
        AppendOutput("\n" + text);
    } else {
        AppendOutput(text);
    }
}

void ChatAIWindow::AppendText(const wxString& text, bool force_style)
{
    AppendOutput(text);
    if (force_style) {
        StyleOutput();
    }
}

void ChatAIWindow::AppendOutput(const wxString& text, bool clear)
{
    CHECK_COND_RET(!text.empty());

    m_stcOutput->SetReadOnly(false);
    if (clear) {
        m_stcOutput->ClearAll();
    }
    m_stcOutput->SetInsertionPointEnd();
    m_stcOutput->SetSelection(m_stcOutput->GetLastPosition(), m_stcOutput->GetLastPosition());
    m_stcOutput->AppendText(text);
    m_stcOutput->SetReadOnly(true);

    if (m_autoScroll) {
        ScrollToEnd();
    }
}

void ChatAIWindow::StyleOutput()
{
    m_markdownStyler->StyleText();
    if (m_autoScroll) {
        ScrollToEnd();
    } else {
        m_stcOutput->ClearSelections();
    }
}

void ChatAIWindow::OnWorkspaceLoaded(clWorkspaceEvent& event) { event.Skip(); }

void ChatAIWindow::OnWorkspaceClosed(clWorkspaceEvent& event) { event.Skip(); }

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

void ChatAIWindow::OnDetachView(wxCommandEvent& event) { wxUnusedVar(event); }

void ChatAIWindow::OnHistory(wxCommandEvent& event)
{
    ChatHistoryDialog dlg(EventNotifier::Get()->TopFrame());
    dlg.ShowModal();
}

bool ChatAIWindow::CurrentEndpointHasHistory() const
{
    auto active_endpoint = llm::Manager::GetInstance().GetActiveEndpoint();
    if (!active_endpoint.has_value()) {
        return false;
    }
    const auto& history = llm::Manager::GetInstance().GetHistoryStore();
    return (history.Count(active_endpoint.value()) > 0) && (m_state == ChatState::kReady);
}

void ChatAIWindow::OnStop(wxCommandEvent& event)
{
    // Save the current session
    llm::Manager::GetInstance().StoreCurrentConverstation(m_stcOutput->GetText(), "__last_session__");
    DoClearOutputView();
    llm::Manager::GetInstance().Restart();

    // Restore the last session
    wxString active_endpoint = m_model_selector->GetEndpoint();
    const auto& store = llm::Manager::GetInstance().GetHistoryStore();
    auto v = store.List(active_endpoint);
    auto iter = std::find_if(
        v.begin(), v.end(), [](const llm::HistoryEntry& entry) { return entry.label == "__last_session__"; });
    if (iter == v.end()) {
        return;
    }

    auto chat = store.Get(active_endpoint, *iter);
    if (!chat.has_value()) {
        return;
    }

    // Load the chat and style the view
    llm::Manager::GetInstance().LoadConversation(chat.value());
    wxBusyCursor bc{};
    wxWindowUpdateLocker locker{m_stcOutput};
    AppendOutput(chat.value().content_, true);
    StyleOutput();
    m_stcInput->CallAfter(&wxStyledTextCtrl::SetFocus);
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

void ChatAIWindow::OnSelection(wxStyledTextEvent& event)
{
    wxUnusedVar(event);
    int current_pos = m_stcInput->GetCurrentPos();
    if (current_pos > m_ccInsertPos && m_ccInsertPos != wxNOT_FOUND) {
        // Delete whatever text the user typed
        m_stcInput->DeleteRange(m_ccInsertPos, current_pos - m_ccInsertPos);
        m_ccInsertPos = wxNOT_FOUND;
    }
    m_stcInput->AutoCompCancel();

    const wxString command = event.GetString();
    if (const auto it = m_commandDispatchTable.find(command); it != m_commandDispatchTable.end()) {
        it->second();
    }
}

void ChatAIWindow::OnCharAdded(wxStyledTextEvent& event)
{
    event.Skip();
    if (!m_stcInput->AutoCompActive()) {
        m_ccInsertPos = wxNOT_FOUND;
    }

    std::vector<wxString> commands;
    commands.reserve(m_commandDispatchTable.size());
    for (const auto& [cmdname, _func] : m_commandDispatchTable) {
        commands.push_back(cmdname);
    }

    std::sort(commands.begin(), commands.end());
    static const wxString candidates = StringUtils::Join(commands, " ");
    if (event.GetKey() == '/') {
        m_ccInsertPos = m_stcInput->PositionBefore(m_stcInput->GetCurrentPos());
        m_stcInput->AutoCompShow(1, candidates);
    }
}

void ChatAIWindow::OnSize(wxSizeEvent& event)
{
    event.Skip();
    if (GetSizer()) {
        Layout();
    }
}

void ChatAIWindow::OnLLMWorkerIdle(clLLMEvent& event)
{
    event.Skip();
    m_state = ChatState::kReady;
    ShowIndicator(false);
    StyleOutput();
    UpdateContextWindowUsage();
}

void ChatAIWindow::OnLLMWorkerBusy(clLLMEvent& event)
{
    event.Skip();
    m_state = ChatState::kWorking;
    ShowIndicator(true);
}

void ChatAIWindow::OnLoadSession(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString active_endpoint = m_model_selector->GetEndpoint();
    const auto& store = llm::Manager::GetInstance().GetHistoryStore();
    auto sessions = store.List(active_endpoint);
    if (sessions.empty()) {
        wxString msg;
        msg << _("There are no stored sessions to load for endpoint: ") << active_endpoint;
        ::clMessageBox(msg);
        return;
    }

    auto m = llm::HistoryEntryHelper::GetAsMap(sessions);
    auto labels = llm::HistoryEntryHelper::GetLabels(sessions);

    wxString sel = ::wxGetSingleChoice(_("Choose a chat to load"), "CodeLite", labels, 0);
    if (sel.empty()) {
        return;
    }

    auto chat = store.Get(active_endpoint, m[sel]);
    if (!chat.has_value()) {
        return;
    }

    // Load the chat
    llm::Manager::GetInstance().LoadConversation(chat.value());
    wxBusyCursor bc{};
    wxWindowUpdateLocker locker{m_stcOutput};
    AppendOutput(chat.value().content_, true);
    StyleOutput();
    m_stcInput->CallAfter(&wxStyledTextCtrl::SetFocus);
}

void ChatAIWindow::OnLoadSessionUI(wxUpdateUIEvent& event)
{
    wxString active_endpoint = m_model_selector->GetEndpoint();
    if (active_endpoint.empty()) {
        event.Enable(false);
        return;
    }
    event.Enable(true);
}

void ChatAIWindow::OnSaveSession(wxCommandEvent& event)
{
    wxUnusedVar(event);
    wxString chat_name = wxGetTextFromUser(_("Enter chat name:"), "CodeLite");
    if (chat_name.empty()) {
        return;
    }
    llm::Manager::GetInstance().StoreCurrentConverstation(m_stcOutput->GetText(), chat_name);
}

void ChatAIWindow::OnSaveSessionUI(wxUpdateUIEvent& event)
{
    wxString active_endpoint = m_model_selector->GetEndpoint();
    event.Enable(!active_endpoint.empty() && !m_stcOutput->IsEmpty());
}

void ChatAIWindow::UpdateContextWindowUsage()
{
    int pos{0};
    if (llm::Manager::GetInstance().GetUsage()) {
        pos = static_cast<int>(llm::Manager::GetInstance().GetUsage()->GetPercentage());
    }
    m_gaugeContextUsed->SetValue(pos);
    m_gaugeContextUsed->SetToolTip(wxString() << _("Context Window Used: ") << pos << "%");
}

void ChatAIWindow::DoCommandClear() { DoClearOutputView(); }

void ChatAIWindow::DoCommandSave()
{
    wxCommandEvent dummy;
    OnSaveSession(dummy);
}

void ChatAIWindow::DoCommandContext()
{
    auto frame = EventNotifier::Get()->TopFrame();
    auto workspace = clWorkspaceManager::Get().GetWorkspace();
#if USE_SFTP
    if (workspace && workspace->IsRemote()) {
        auto paths =
            SFTPBrowserDlg::ShowPicker(_("Choose Context Files"), workspace->GetSshAccount(), workspace->GetDir());
        if (!paths) {
            return;
        }

        size_t count{0};
        wxBusyCursor bc{};
        wxArrayString files = std::move(paths.value());
        clINFO() << "Loading context files:" << files << endl;
        for (const auto& file : files) {
            wxMemoryBuffer content;
            if (clSFTPManager::Get().AwaitReadFile(file, workspace->GetSshAccount(), &content)) {
                wxString data{(const unsigned char*)content.GetData(), wxConvUTF8, content.GetDataLen()};
                llm::Manager::GetInstance().AddSystemMessage(data);
                ++count;
            }
        }
        clGetManager()->SetStatusMessage(wxString() << _("Added ") << count << _(" files to the context"));
        // Read all the files and add them to the context.
        return;
    }
#endif

    // Local mode: load markdown files per user request and update the system message.
    wxString wildcard = wxT("Markdown files (*.md)|*.md|All files (*)|*");
    wxFileDialog openFileDialog(frame,
                                _("Choose Context Files"),
                                workspace ? workspace->GetDir() : wxString{},
                                "",
                                wildcard,
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE);
    if (openFileDialog.ShowModal() != wxID_OK) {
        return;
    }

    wxArrayString paths;
    openFileDialog.GetPaths(paths);

    // Read the local files content
    wxBusyCursor bc{};
    size_t count{0};
    for (const auto& path : paths) {
        wxString data;
        if (FileUtils::ReadFileContent(path, data)) {
            llm::Manager::GetInstance().AddSystemMessage(data);
        }
        ++count;
    }

    clGetManager()->SetStatusMessage(wxString() << _("Added ") << count << _(" files to the context"));
}
