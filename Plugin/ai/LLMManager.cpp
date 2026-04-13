#include "LLMManager.hpp"

#include "CTags.hpp"
#include "FileManager.hpp"
#include "Keyboard/clKeyboardManager.h"
#include "ai/ToolTrustLevelDlg.hpp"
#include "assistant/EnvExpander.hpp"
#include "assistant/assistant.hpp"
#include "clWorkspaceManager.h"
#include "cl_command_event.h"
#include "cl_standard_paths.h"
#include "codelite_events.h"
#include "environmentconfig.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileextmanager.h"
#include "fileutils.h"
#include "globals.h"
#include "resources/clXmlResource.hpp"

#include <algorithm>
#include <future>
#include <wx/app.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/richmsgdlg.h>
#include <wx/xrc/xmlres.h>

#define CHECK_INITIALISE_CALLED() \
    if (!m_initialise_called) {   \
        return;                   \
    }

namespace llm
{
namespace
{
constexpr float kConfigVersion = 1.0;
constexpr const char* kConfigVersionProperty = "_version";
static const std::string kSystemMessageRetryProtocol = R"#(**ERROR HANDLING PROTOCOL**

When encountering errors:

1. **First error:** Analyze, attempt one fix, retry once
2. **Second error (same type):** Try one alternative approach if available
3. **Third error:** **STOP**. Report the issue. Ask user for guidance.**
Apply to: permission denied, tool failures, file errors, invalid parameters, command failures.
**Never** loop indefinitely on repeated failures. Three attempts maximum before escalating to user.)#";

/**
 * @brief RAII guard that restores an atomic boolean to a specified value upon destruction.
 *
 * This struct acquires a reference to an `std::atomic_bool` and remembers a target value
 * at construction time. When the `AtomiBoolLocker` is destroyed (goes out of scope), it
 * atomically stores the remembered value back into the boolean, ensuring cleanup even if
 * exceptions occur.
 *
 * @param b Reference to the `std::atomic_bool` to be managed.
 * @param v The value to restore to `b` when this locker is destroyed.
 *
 * @note This struct does not return a value; it is intended to be used as a scoped guard.
 * @note No exceptions are thrown by the constructor or destructor (atomic store is noexcept).
 *
 * @see std::atomic_bool
 * @see RAII (Resource Acquisition Is Initialization)
 */
struct AtomiBoolLocker {
    std::atomic_bool& bool_;
    bool value_;
    explicit AtomiBoolLocker(std::atomic_bool& b, bool v)
        : bool_{b}
        , value_{v}
    {
    }
    ~AtomiBoolLocker() { bool_.store(value_); }
};

wxString TruncateText(const wxString& text, size_t size = 100)
{
    if (text.size() >= size) {
        return text.Mid(0, size - 3) << "...";
    }
    return text;
}

static std::atomic_bool busy_cusrsor{false};
/**
 * @brief RAII wrapper for managing a wxBusyCursor instance with thread-safe activation.
 *
 * This struct ensures that a busy cursor (hourglass/wait cursor) is displayed only when
 * running on the main GUI thread and when no other BusyCursor is already active. The
 * cursor is automatically restored when the BusyCursor object goes out of scope.
 *
 * @note This struct relies on a global `busy_cusrsor` flag to prevent nested busy cursors.
 * @note The busy cursor is only activated if called from the main thread (checked via wxThread::IsMain()).
 *
 * @see wxBusyCursor
 */
struct BusyCursor {
    std::unique_ptr<wxBusyCursor> bc;
    BusyCursor()
    {
        if (wxThread::IsMain() && busy_cusrsor == false) {
            busy_cusrsor = true;
            bc = std::make_unique<wxBusyCursor>();
        }
    }
    ~BusyCursor()
    {
        if (bc) {
            busy_cusrsor = false;
        }
    }
};

struct TaskDropper {
    llm::ThreadTask& task;
    TaskDropper(llm::ThreadTask& t)
        : task{t}
    {
    }
    ~TaskDropper()
    {
        if (!task.collector) {
            return;
        }

        auto* manager = &llm::Manager::GetInstance();
        // Pass the collector to the main thread so it will deleted by the main thread.
        manager->CallAfter(&llm::Manager::DeleteCollector, task.collector);
        task.collector = nullptr;
    }
};

struct SharedState {
    /// used for detecting loops in the model.
    std::vector<std::string> last_tokens;
    size_t total_batch_count{1};
    size_t current_batch{1};
};

void NotifyDoneWithError(wxEvtHandler* owner, const std::string& message)
{
    CHECK_PTR_RET(owner);
    clLLMEvent event{wxEVT_LLM_OUTPUT_DONE};
    event.SetResponseRaw(message);
    event.SetIsError(true);
    owner->AddPendingEvent(event);
}

void NotifyRequestCancelled(wxEvtHandler* owner, const std::string& message)
{
    CHECK_PTR_RET(owner);
    clLLMEvent event{wxEVT_LLM_OUTPUT_DONE};
    event.SetResponseRaw(message);
    event.SetRequestCancelled(true);
    owner->AddPendingEvent(event);
}

/**
 * @brief Creates a short label from the first meaningful line of text.
 *
 * The function splits the input into lines, trims leading and trailing whitespace from each line,
 * skips empty lines and lines that start with "**", and returns the first remaining line truncated
 * to at most 50 characters.
 *
 * @param text const wxString& The input text to scan for a label.
 *
 * @return std::optional<wxString> The first non-empty, non-markup line truncated to 50 characters,
 *         or std::nullopt if no suitable line is found.
 */
std::optional<wxString> MakeLabelFromText(const wxString& text)
{
    auto lines = wxStringTokenize(text, "\n", wxTOKEN_STRTOK);
    for (wxString& line : lines) {
        line.Trim().Trim(false);
        if (line.empty() || line.StartsWith(wxT("❰"))) {
            continue;
        }
        return line.Mid(0, 50);
    }
    return std::nullopt;
}

} // namespace

constexpr const char* kPlaceHolderEditorSelection = "{{current_selection}}";
constexpr const char* kPlaceHolderCurrentFileFullPath = "{{current_file_fullpath}}";
constexpr const char* kPlaceHolderCurrentFileExt = "{{current_file_ext}}";
constexpr const char* kPlaceHolderCurrentFileDir = "{{current_file_dir}}";
constexpr const char* kPlaceHolderCurrentFileName = "{{current_file_name}}";
constexpr const char* kPlaceHolderCurrentFileLang = "{{current_file_lang}}";
constexpr const char* kPlaceHolderCurrentFileContent = "{{current_file_content}}";
constexpr const char* kPlaceHolderWorkspacePath = "{{workspace_path}}";
constexpr const char* kPlaceHolderTempDir = "{{temp_dir}}";

static std::vector<wxString> kPlaceHolders = {kPlaceHolderEditorSelection,
                                              kPlaceHolderCurrentFileFullPath,
                                              kPlaceHolderCurrentFileExt,
                                              kPlaceHolderCurrentFileDir,
                                              kPlaceHolderCurrentFileName,
                                              kPlaceHolderCurrentFileLang,
                                              kPlaceHolderCurrentFileContent,
                                              kPlaceHolderWorkspacePath,
                                              kPlaceHolderTempDir};

// ==---------------------
// Client
// ==---------------------

Manager& Manager::GetInstance()
{
    static Manager instance;
    return instance;
}

Manager::~Manager()
{
    Stop();
    EventNotifier::Get()->Unbind(wxEVT_FILE_SAVED, &Manager::OnFileSaved, this);
    EventNotifier::Get()->Unbind(wxEVT_CONTEXT_MENU_EDITOR, &Manager::OnEditorContextMenu, this);
}

void Manager::Initialise()
{
    m_chatAI = std::make_unique<ChatAI>();
    m_worker_thread_running = std::make_shared<std::atomic_bool>(false);
    EventNotifier::Get()->Bind(wxEVT_FILE_SAVED, &Manager::OnFileSaved, this);
    EventNotifier::Get()->Bind(wxEVT_INIT_DONE, [this](wxCommandEvent& e) {
        e.Skip();
        // Mark the instance as initialised, this needs to be done before we call 'Start'
        // or it will fail.
        m_initialise_called = true;
        CompleteInitialisation();

        // Now that all the plugins are loaded, start the agent.
        Start();
    });
}

bool Manager::IsAvailable()
{
    // return true if we have an active endpoint.
    return GetActiveEndpoint().has_value();
}

void Manager::OnEditorContextMenu(clContextMenuEvent& event)
{
    event.Skip();
    if (IsAvailable()) {
        // Load the LLM generation sub-menu
        wxMenu* menu = event.GetMenu();
        wxMenu* ai_menu = clXmlResource::Get().LoadMenu("editor_context_menu_llm_generation");
        menu->PrependSeparator();

        auto item = menu->Prepend(wxID_ANY, _("AI-Powered Options"), ai_menu);
        item->SetBitmap(clGetManager()->GetStdIcons()->LoadBitmap("wand"));
        ai_menu->Bind(wxEVT_MENU, &Manager::OnGenerateDocString, this, XRCID("lsp_document_scope"));
    }
}

void Manager::DeleteCollector(ResponseCollector* collector) { wxDELETE(collector); }

void Manager::WorkerMain()
{
    // Capture a copy of the shared_ptr to keep it alive for this thread,
    // even if the Manager is destroyed or the thread is detached
    auto worker_thread_running = m_worker_thread_running;

    CHECK_PTR_RET(m_client);

    // Create our own copy to make sure that when reloading configuration, this instance
    // is un-affected.
    auto client = m_client;

    // Helper class to pass the collector to the main thread from the ThreadTask class
    // once it drops out of scope. This way we ensure that the collector will be destroyed
    // without pending events.
    clDEBUG() << "LLM Worker thread started" << endl;
    worker_thread_running->store(true);
    bool cont{true};

    clLLMEvent idle_event{wxEVT_LLM_WORKER_IDLE};
    AddPendingEvent(idle_event);

    while (cont && !client->IsInterrupted()) {
        m_worker_busy.store(false);
        ThreadTask task;
        auto res = m_queue.ReceiveTimeout(50, task);
        if (res == wxMSGQUEUE_TIMEOUT) {
            continue;
        }

        clDEBUG() << "Handling chat request:" << TruncateText(task.prompt_array[0]) << endl;
        m_worker_busy.store(true);
        TaskDropper dropper{task};

        // Process the the task
        bool saved_thinking_state{false};
        wxEvtHandler* owner = task.GetEventSink();

        clLLMEvent event_starting{wxEVT_LLM_CHAT_STARTED};
        owner->AddPendingEvent(event_starting);

        clLLMEvent busy_event{wxEVT_LLM_WORKER_BUSY};
        AddPendingEvent(busy_event);

        SharedState shared_state;
        shared_state.total_batch_count = task.prompt_array.size();
        shared_state.current_batch = 1;

        bool abort_loop{false};
        auto cancellation_token = task.cancellation_token;
        for (std::string prompt : task.prompt_array) {
            if (abort_loop) {
                break;
            }

            try {
                clDEBUG() << "Client (URL:" << client->GetUrl() << ", Model:" << client->GetModel()
                          << ") is processing the request" << endl;
                client->Chat(
                    std::move(prompt),
                    [client, cancellation_token, &abort_loop, &shared_state, &saved_thinking_state, owner](
                        std::string message, assistant::Reason reason, bool thinking) -> bool {
                        // Check various options that the chat was cancelled.
                        if (client->IsInterrupted() || (cancellation_token && cancellation_token->IsCancelled())) {
                            NotifyRequestCancelled(owner, "\n\n** Request cancelled by the user. **\n\n");
                            abort_loop = true;
                            return false;
                        }

                        if (saved_thinking_state != thinking) {
                            // we switched state
                            if (thinking) {
                                // the new state is "thinking"
                                clLLMEvent think_started{wxEVT_LLM_THINK_SATRTED};
                                owner->AddPendingEvent(think_started);
                            } else {
                                clLLMEvent think_ended{wxEVT_LLM_THINK_ENDED};
                                owner->AddPendingEvent(think_ended);
                            }
                        }

                        saved_thinking_state = thinking;
                        switch (reason) {
                        case assistant::Reason::kToolAllowed: {
                            clLLMEvent event{wxEVT_LLM_OUTPUT};
                            event.SetOutputReason(reason);
                            event.SetResponseRaw(message);
                            owner->AddPendingEvent(event);
                        } break;
                        case assistant::Reason::kMaxTokensReached: {
                            clLLMEvent event{wxEVT_LLM_MAX_GENERATED_TOKENS};
                            event.SetOutputReason(reason);
                            event.SetResponseRaw(message);
                            owner->AddPendingEvent(event);
                            abort_loop = true;
                        } break;
                        case assistant::Reason::kToolDenied: {
                            clLLMEvent event{wxEVT_LLM_OUTPUT};
                            event.SetOutputReason(reason);
                            event.SetResponseRaw(message);
                            owner->AddPendingEvent(event);
                        } break;
                        case assistant::Reason::kFatalError: {
                            clERROR() << "LLM response ended with an error:" << wxString::FromUTF8(message) << endl;
                            NotifyDoneWithError(owner, message);
                            abort_loop = true;
                            return false;
                        } break;
                        case assistant::Reason::kDone: {
                            if (cancellation_token && !cancellation_token->Incr()) {
                                NotifyDoneWithError(owner, "\n\n** Maximum number of tokens reached. **\n\n");
                                abort_loop = true;
                                return false;
                            }
                            if (shared_state.current_batch == shared_state.total_batch_count) {
                                // No more batches to process, fire the DONE event.
                                clLLMEvent event{wxEVT_LLM_OUTPUT_DONE};
                                event.SetOutputReason(reason);
                                event.SetResponseRaw(message);
                                owner->AddPendingEvent(event);
                            } else {
                                // We have more batches to process, treat this as a
                                // normal "output" event.
                                shared_state.current_batch++;
                                clLLMEvent event{wxEVT_LLM_OUTPUT};
                                event.SetOutputReason(reason);
                                event.SetResponseRaw(message);
                                owner->AddPendingEvent(event);
                            }
                        } break;
                        case assistant::Reason::kLogNotice:
                            clDEBUG1() << message << endl;
                            break;
                        case assistant::Reason::kCancelled: {
                            NotifyRequestCancelled(owner, "\n\n** Request cancelled by caller. **\n\n");
                            abort_loop = true;
                            return false;
                        } break;
                        case assistant::Reason::kLogDebug:
                            clDEBUG() << message << endl;
                            break;
                        case assistant::Reason::kRequestCost:
                            clDEBUG() << message << endl;
                            break;
                        case assistant::Reason::kPartialResult: {
                            clLLMEvent event{wxEVT_LLM_OUTPUT};
                            event.SetOutputReason(reason);
                            event.SetResponseRaw(message);
                            owner->AddPendingEvent(event);
                            if (cancellation_token && !cancellation_token->Incr()) {
                                NotifyDoneWithError(owner, "\n\n** Maximum number of tokens reached. **\n\n");
                                abort_loop = true;
                                return false;
                            }
                        } break;
                        }
                        // continue
                        return true;
                    },
                    task.options);
            } catch (std::exception& e) {
                clERROR() << "LLM worker that got an exception." << e.what() << endl;
                wxString errmsg;
                errmsg << "\n\n** Request ended with an error. " << e.what() << " **\n\n";
                NotifyDoneWithError(owner, errmsg.ToStdString(wxConvUTF8));
                cont = false;
                break;
            }
        } // Prompt loop

        clLLMEvent idle_event{wxEVT_LLM_WORKER_IDLE};
        AddPendingEvent(idle_event);

    } // Main Loop
    clDEBUG() << "LLM worker thread exited" << endl;
    worker_thread_running->store(false);
    m_worker_busy.store(false);
}

void Manager::CleanupAfterWorkerExit()
{
    // Pull all items from the queue and notify about "Stop"
    if (!wxThread::IsMain()) {
        clERROR() << "CleanupAfterWorkerExit can only be called from the main thread" << endl;
        return;
    }

    // Drain the worker thread input queue
    ThreadTask w;
    while (m_queue.ReceiveTimeout(1, w) == wxMSGQUEUE_NO_ERROR) {
        TaskDropper dropper{w};
        clLLMEvent event{wxEVT_LLM_OUTPUT_DONE};
        event.SetResponseRaw("LLM is going down");
        w.GetEventSink()->AddPendingEvent(event);
    }

    m_queue.Clear();
    DetachWorkerThread();

    // Create a new "is worker running" variable.
    m_worker_thread_running = std::make_shared<std::atomic_bool>(false);
}

bool Manager::DetachWorkerThread()
{
    if (!m_worker_thread) {
        return true;
    }

    // Worker is still busy (e.g. in-flight HTTP request), detach to avoid blocking the UI
    m_worker_thread->detach();
    m_worker_thread.reset();

    clDEBUG() << "LLM Worker thread detached" << endl;
    return true;
}

void Manager::PostTask(ThreadTask task)
{
    if (!m_worker_thread_running->load()) {
        clDEBUG() << "Restarting worker thread." << endl;
        CleanupAfterWorkerExit();
        Start();
    }

    ReplacePlaceHolders(task);
    m_queue.Post(std::move(task));
}

void Manager::ReplacePlaceHolders(ThreadTask& task)
{
    const std::vector<std::function<void(wxString&)>> kManipulators = {
        [](wxString& prompt) {
            auto editor = clGetManager()->GetActiveEditor();
            if (!editor) {
                prompt.Replace(kPlaceHolderEditorSelection, wxEmptyString);
                return;
            }
            prompt.Replace(kPlaceHolderEditorSelection, editor->GetSelection());
        },
        [](wxString& prompt) {
            auto editor = clGetManager()->GetActiveEditor();
            if (!editor) {
                prompt.Replace(kPlaceHolderCurrentFileFullPath, wxEmptyString);
                return;
            }
            prompt.Replace(kPlaceHolderCurrentFileFullPath, editor->GetRemotePathOrLocal());
        },
        [](wxString& prompt) {
            auto editor = clGetManager()->GetActiveEditor();
            if (!editor) {
                prompt.Replace(kPlaceHolderCurrentFileExt, wxEmptyString);
                return;
            }
            prompt.Replace(kPlaceHolderCurrentFileExt, editor->GetFileName().GetExt());
        },
        [](wxString& prompt) {
            auto editor = clGetManager()->GetActiveEditor();
            if (!editor) {
                prompt.Replace(kPlaceHolderCurrentFileDir, ::wxGetCwd());
                return;
            }

            wxString dir;
            if (editor->IsRemoteFile()) {
                dir = editor->GetRemotePathOrLocal().BeforeLast('/');
            } else {
                dir = editor->GetFileName().GetPath();
            }
            prompt.Replace(kPlaceHolderCurrentFileDir, dir);
        },
        [](wxString& prompt) {
            auto editor = clGetManager()->GetActiveEditor();
            if (!editor) {
                prompt.Replace(kPlaceHolderCurrentFileName, wxEmptyString);
                return;
            }

            wxString fullname;
            if (editor->IsRemoteFile()) {
                fullname = editor->GetRemotePathOrLocal().AfterLast('/');
            } else {
                fullname = editor->GetFileName().GetFullName();
            }
            prompt.Replace(kPlaceHolderCurrentFileName, fullname);
        },
        [](wxString& prompt) {
            auto editor = clGetManager()->GetActiveEditor();
            if (!editor) {
                prompt.Replace(kPlaceHolderCurrentFileLang, wxEmptyString);
                return;
            }

            auto lang = FileExtManager::GetLanguageFromType(FileExtManager::GetType(editor->GetRemotePathOrLocal()));
            lang = lang.Lower();
            lang.Replace(" ", "");
            prompt.Replace(kPlaceHolderCurrentFileLang, lang);
        },
        [](wxString& prompt) {
            auto editor = clGetManager()->GetActiveEditor();
            if (!editor) {
                prompt.Replace(kPlaceHolderCurrentFileContent, wxEmptyString);
                return;
            }
            prompt.Replace(kPlaceHolderCurrentFileContent, editor->GetEditorText());
        },
        [](wxString& prompt) {
            if (!clWorkspaceManager::Get().IsWorkspaceOpened()) {
                prompt.Replace(kPlaceHolderWorkspacePath, ::wxGetCwd());
                return;
            }
            prompt.Replace(kPlaceHolderWorkspacePath, clWorkspaceManager::Get().GetWorkspace()->GetDir());
        },
        [](wxString& prompt) { prompt.Replace(kPlaceHolderTempDir, clStandardPaths::Get().GetTempDir()); }};

    // Apply the manipulators on the prompt
    for (auto& p : task.prompt_array) {
        wxString prompt = wxString::FromUTF8(p);
        for (auto& manipulator : kManipulators) {
            manipulator(prompt);
            p = prompt.ToStdString(wxConvUTF8);
        }
    }
}

void Manager::Chat(wxEvtHandler* owner,
                   const wxArrayString& prompts,
                   std::shared_ptr<CancellationToken> cancel_token,
                   ChatOptions options)
{
    // Post 1 job with multiple prompts.
    ThreadTask task{.options = options, .owner = owner, .cancellation_token = cancel_token};
    for (const auto& prompt : prompts) {
        task.prompt_array.push_back(prompt.ToStdString(wxConvUTF8));
    }
    PostTask(std::move(task));
}

void Manager::Chat(ResponseCollector* collector,
                   const wxArrayString& prompts,
                   std::shared_ptr<CancellationToken> cancel_token,
                   ChatOptions options)
{
    // Post 1 job with multiple prompts.
    ThreadTask task{.options = options, .cancellation_token = cancel_token, .collector = collector};
    for (const auto& prompt : prompts) {
        task.prompt_array.push_back(prompt.ToStdString(wxConvUTF8));
    }
    PostTask(std::move(task));
}

void Manager::Chat(wxEvtHandler* owner,
                   const wxString& prompt,
                   std::shared_ptr<CancellationToken> cancel_token,
                   ChatOptions options)
{
    ThreadTask task{.prompt_array = {prompt.ToStdString(wxConvUTF8)},
                    .options = options,
                    .owner = owner,
                    .cancellation_token = cancel_token};
    PostTask(std::move(task));
}

void Manager::Chat(ResponseCollector* collector,
                   const wxString& prompt,
                   std::shared_ptr<CancellationToken> cancel_token,
                   ChatOptions options)
{
    ThreadTask task{.prompt_array = {prompt.ToStdString(wxConvUTF8)},
                    .options = options,
                    .cancellation_token = cancel_token,
                    .collector = collector};
    PostTask(std::move(task));
}

std::once_flag config_create_once_flag;
assistant::Config Manager::MakeConfig()
{
    std::call_once(config_create_once_flag, [this]() {
        // Should never fail.
        m_default_config =
            assistant::ConfigBuilder::FromContent(kDefaultSettings.ToStdString(wxConvUTF8)).config_.value();

        // Redirect the library logs to our logging machinery.
        assistant::SetLogSink([](assistant::LogLevel level, std::string msg) {
            switch (level) {
            case assistant::LogLevel::kError:
                clERROR() << "LLM:" << wxString::FromUTF8(msg) << endl;
                break;
            case assistant::LogLevel::kWarning:
                clWARNING() << "LLM:" << wxString::FromUTF8(msg) << endl;
                break;
            case assistant::LogLevel::kInfo:
            case assistant::LogLevel::kDebug:
            case assistant::LogLevel::kTrace:
                clDEBUG1() << "LLM:" << wxString::FromUTF8(msg) << endl;
                break;
            }
        });
    });

    auto res = CreateOrOpenConfig();
    if (!res.ok()) {
        return m_default_config;
    }

    // Apply environment variables before we proceed
    EnvSetter env;
    auto result = assistant::ConfigBuilder::FromFile(res.value().ToStdString(wxConvUTF8));
    if (!result.ok()) {
        if (::wxIsMainThread()) {
            ::wxMessageBox(result.errmsg_, "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
            return assistant::ConfigBuilder::FromContent(kDefaultSettings.ToStdString(wxConvUTF8)).config_.value();
        }
    }
    return result.config_.value();
}

void Manager::Restart()
{
    CHECK_INITIALISE_CALLED();
    Stop();
    Start();
}

void Manager::Stop()
{
    CHECK_INITIALISE_CALLED();
    auto bc = std::make_unique<BusyCursor>();
    CHECK_PTR_RET(m_client);

    {
        // Notify all our subscribers that we are going down.
        std::lock_guard lk{m_mutex};
        for (auto flag : m_termination_flags) {
            flag->store(true);
        }
        m_termination_flags.clear();
    }

    m_clientStopInProgress.store(true);
    m_client->Interrupt();
    CleanupAfterWorkerExit();
    m_clientStopInProgress.store(false);
    m_client.reset();

    clLLMEvent stop_event{wxEVT_LLM_STOPPED};
    stop_event.SetEventObject(this);
    AddPendingEvent(stop_event);
}

CanInvokeToolResult Manager::CanRunTool(const std::string& tool_name, [[maybe_unused]] assistant::json args)
{
    if (GetInstance().GetConfig().IsToolTrustedFor(
            tool_name, [](const wxString& pattern) -> bool { return pattern == "*"; })) {
        return CanInvokeToolResult{
            .can_invoke = true,
            .reason = "Tool is trusted",
        };
    }

    wxString base_message;
    base_message << _("The model wants to run the tool: `") << tool_name << _("`\n");
    return GetInstance().PromptUserYesNoTrustQuestion(base_message, [tool_name]() {
        wxString message;
        wxString tool_name_utf8 = wxString::FromUTF8(tool_name);
        message << _("Would you like to persist trust for the tool '") << tool_name_utf8 << _("' for future sessions?");
        bool persist = ::clMessageBox(message, _("Confirm"), wxYES_NO | wxICON_QUESTION | wxNO_DEFAULT) == wxYES;
        GetInstance().GetConfig().AddTrustedTool(tool_name_utf8, "*", persist);
        if (persist) {
            GetInstance().GetConfig().Save(false);
        }
    });
}

void Manager::Start(std::shared_ptr<assistant::ClientBase> client)
{
    CHECK_INITIALISE_CALLED();
    auto bc = std::make_unique<BusyCursor>();
    m_client_config = MakeConfig();

    // MakeClient that accepts a Config object can not fail.
    bool is_claude =
        m_client_config.GetEndpoint() && m_client_config.GetEndpoint()->type_ == assistant::EndpointKind::anthropic;
    m_client = client == nullptr ? assistant::MakeClient(m_client_config).value_or(nullptr) : client;
    CHECK_PTR_RET(m_client);

    // Initialise the function table.
    PopulateBuiltInFunctions(m_client->GetFunctionTable());
    m_client->GetFunctionTable().Merge(GetPluginFunctionTable());

    // Add external MCP tools.
    m_client->GetFunctionTable().ReloadMCPServers(&m_client_config);

    // Check if this is a stock model and associate the pricing for it.
    auto active_endpoint = m_client_config.GetEndpoint();
    if (active_endpoint) {
        auto pricing = assistant::FindPricing(active_endpoint->model_);
        if (pricing.has_value()) {
            m_client->SetPricing(pricing.value());
        } else {
            // TODO: add support for user's custom pricing.
        }
    }

    m_client->SetCachingPolicy(GetConfig().GetCachePolicy());
    m_client->SetToolInvokeCallback(&Manager::CanRunTool);
    m_client->ClearSystemMessages();
    m_client->AddSystemMessage(kSystemMessageRetryProtocol);

    // Start the worker thread
    m_worker_thread = std::make_unique<std::thread>([this]() { WorkerMain(); });

    clLLMEvent start_event{wxEVT_LLM_STARTED};
    start_event.SetEventObject(this);
    AddPendingEvent(start_event);
}

bool Manager::ReloadConfig(std::optional<wxString> config_content, bool prompt)
{
    if (prompt && ::clMessageBox(_("Reloading the configuration will restart "
                                   "the LLM client.\nContinue?"),
                                 "CodeLite",
                                 wxICON_QUESTION | wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxCENTER) != wxYES) {
        return false;
    }

    wxString content;
    if (config_content.has_value()) {
        content = std::move(config_content.value());
    } else {
        const WriteOptions opts{.ignore_workspace = true};
        content = FileManager::ReadSettingsFileContent(kAssistantConfigFile, opts).value_or(kDefaultSettings);
    }

    {
        // Check that the file does not contain any un-resolved environment variables
        auto result = ValidateConfigFile(content);
        if (!result.ok()) {
            if (::wxIsMainThread()) {
                wxString errmsg;
                errmsg << _("Failed to reload AI configuration:\n") << result.message();
                ::clMessageBox(errmsg, "CodeLite", wxICON_WARNING | wxOK | wxCENTRE);
            }
            return false;
        }
    }

    // First stop the running instance
    Stop();

    EnvSetter env;
    auto result = assistant::ConfigBuilder::FromContent(content.ToStdString(wxConvUTF8));
    if (!result.ok()) {
        if (::wxIsMainThread()) {
            ::clMessageBox(result.errmsg_, "CodeLite", wxICON_ERROR | wxOK | wxCENTER);
        }
        return false;
    }

    m_client_config = std::move(result.config_.value());
    // Now we can replace the client instance
    auto client = assistant::MakeClient(m_client_config).value_or(nullptr);
    if (!client) {
        clERROR() << "Could not create new LLM client!" << endl;
        return false;
    }

    // Start the new client
    Start(client);
    return true;
}

void Manager::ClearHistory()
{
    CHECK_PTR_RET(m_client);
    m_client->ClearHistoryMessages();
}

std::optional<llm::Conversation> Manager::NewConversation(const wxString& conversation_text) const
{
    if (!m_client) {
        return std::nullopt;
    }

    auto label = MakeLabelFromText(conversation_text);
    if (label.has_value()) {
        return llm::Conversation(m_client->GetHistory(), conversation_text, label.value());
    }
    return std::nullopt;
}

void Manager::LoadConversation(const llm::Conversation& conversation)
{
    CHECK_PTR_RET(m_client);
    m_client->SetHistory(conversation.messages_);
}

wxArrayString Manager::ListEndpoints()
{
    const auto& endpoints = m_client_config.GetEndpoints();
    wxArrayString result;
    result.reserve(endpoints.size());
    for (auto ep : endpoints) {
        result.Add(ep->url_);
    }
    return result;
}

std::optional<std::pair<wxString, wxArrayString>> Manager::GetEndpointModels(const wxString& endpoint)
{
    ASSIGN_OPT_OR_RETURN(auto j, GetConfigAsJSON(), std::nullopt);
    try {
        std::pair<wxString, wxArrayString> result;
        std::string endpoint_str = endpoint.ToStdString(wxConvUTF8);
        auto endpoint_json = j["endpoints"][endpoint_str];
        auto active_model = endpoint_json["model"].get<std::string>();
        result.first = wxString::FromUTF8(active_model);
        if (endpoint_json.contains("models")) {
            std::vector<std::string> all_models = endpoint_json["models"].get<std::vector<std::string>>();
            std::set<std::string> models_set;
            models_set.insert(all_models.begin(), all_models.end());
            models_set.insert(active_model);
            for (const auto& model_name : models_set) {
                auto m = wxString::FromUTF8(model_name);
                result.second.Add(m);
            }
        } else {
            result.second.Add(result.first);
        }
        return result;
    } catch (const std::exception& e) {
        return std::nullopt;
    }
}

void Manager::SetEndpointModel(const wxString& endpoint, const wxString& model)
{
    ASSIGN_OPT_OR_RETURN(auto j, GetConfigAsJSON(), );
    try {
        std::string endpoint_str = endpoint.ToStdString(wxConvUTF8);
        std::string model_str = model.ToStdString(wxConvUTF8);

        auto& endpoint_json = j["endpoints"][endpoint_str];
        endpoint_json["model"] = model_str;
        std::vector<std::string> all_models;
        if (!endpoint_json.contains("models")) {
            all_models.push_back(model_str);
        } else {
            all_models = endpoint_json["models"].get<std::vector<std::string>>();
            auto iter = std::find_if(all_models.begin(), all_models.end(), [model_str](const std::string& m) -> bool {
                return m == model_str;
            });

            if (iter == all_models.end()) {
                all_models.push_back(model_str);
            }
        }
        endpoint_json["models"] = all_models;
        if (WriteConfigFile(std::move(j))) {
            HandleConfigFileUpdated();
        }
    } catch ([[maybe_unused]] const std::exception& e) {
    }
}

bool Manager::SetActiveEndpoint(const wxString& endpoint)
{
    ASSIGN_OPT_OR_RETURN(auto j, GetConfigAsJSON(), false);
    if (!j.contains("endpoints")) {
        return false;
    }

    auto endpoints = j["endpoints"];
    bool endpoint_found{false};
    auto endpoints_items = endpoints.items();
    llm::json new_endpoints;
    for (auto& [name, val] : endpoints_items) {
        if (!endpoint_found) {
            endpoint_found = (name == endpoint);
        }
        val["active"] = (name == endpoint);
        new_endpoints[name] = val;
    }

    if (!endpoint_found) {
        return false;
    }

    j["endpoints"] = new_endpoints;
    if (WriteConfigFile(std::move(j))) {
        HandleConfigFileUpdated();
        return true;
    }
    return false;
}

std::optional<wxString> Manager::GetActiveEndpoint() const
{
    auto active_endpoint = m_client_config.GetEndpoint();
    if (!active_endpoint) {
        return std::nullopt;
    }
    return active_endpoint->url_;
}

std::optional<llm::json> Manager::GetConfigAsJSON()
{
    const WriteOptions opts{.ignore_workspace = true};
    wxString config_content =
        FileManager::ReadSettingsFileContent(kAssistantConfigFile, opts).value_or(kDefaultSettings);
    try {
        return llm::json::parse(config_content.ToStdString(wxConvUTF8));

    } catch (const std::exception& e) {
        clERROR() << "Failed to parse JSON file:" << kAssistantConfigFile << "." << e.what() << endl;
        return std::nullopt;
    }
}

void Manager::AddNewEndpoint(const llm::EndpointData& d)
{
    try {
        ASSIGN_OPT_OR_RETURN(auto j, GetConfigAsJSON(), );
        if (!j.contains("endpoints")) {
            j["endpoints"] = {};
        }

        auto endpoints = j["endpoints"];
        if (endpoints.contains(d.url)) {
            endpoints.erase(d.url);
        }

        llm::json new_endpoint;
        new_endpoint["active"] = GetActiveEndpoint().has_value() ? false : true;

        if (d.client_type == kClientTypeAnthropic) {
            llm::json http_headers;
            http_headers["x-api-key"] = d.api_key.value_or("<INSERT_API_KEY>");
            new_endpoint["http_headers"] = http_headers;
            new_endpoint["verify_server_ssl"] = false;

        } else if (d.client_type == kClientTypeOllama && d.api_key.has_value()) {
            // Ollama cloud
            llm::json http_headers;
            http_headers["Authorization"] = "Bearer " + d.api_key.value();
            new_endpoint["http_headers"] = http_headers;
            new_endpoint["verify_server_ssl"] = false;

        } else if (d.client_type == kClientTypeOllama) {
            // Ollama local
            llm::json http_headers;
            http_headers["Host"] = "127.0.0.1";
            new_endpoint["http_headers"] = http_headers;
        } else if (d.client_type == kClientTypeOpenAI) {
            llm::json http_headers;
            http_headers["Authorization"] = "Bearer " + d.api_key.value_or("<INSERT_API_KEY>");
            new_endpoint["http_headers"] = http_headers;
            new_endpoint["verify_server_ssl"] = false;
        }

        new_endpoint["type"] = d.client_type;
        new_endpoint["model"] = d.model;
        new_endpoint["models"] = std::vector{d.model};
        if (d.context_size.has_value()) {
            new_endpoint["context_size"] = d.context_size.value();
        }

        if (d.max_tokens.has_value()) {
            new_endpoint["max_tokens"] = d.max_tokens.value();
        }

        j["endpoints"][d.url] = new_endpoint;
        if (WriteConfigFile(std::move(j))) {
            HandleConfigFileUpdated();
        }

    } catch (const std::exception& e) {
        clERROR() << "Failed to add new endpoint:" << e.what() << endl;
    }
}

void Manager::AddNewMcp(const llm::SSEMcp& d)
{
    try {
        ASSIGN_OPT_OR_RETURN(auto j, GetConfigAsJSON(), );
        if (!j.contains("mcp_servers")) {
            j["mcp_servers"] = {};
        }

        auto mcp_servers = j["mcp_servers"];
        if (mcp_servers.contains(d.name)) {
            mcp_servers.erase(d.name);
        }

        llm::json new_mcp;
        new_mcp["type"] = "sse";
        new_mcp["enabled"] = true;
        new_mcp["baseurl"] = d.base_url;
        new_mcp["endpoint"] = d.endpoint;
        new_mcp["headers"] = d.headers;
        new_mcp["auth_token"] = d.auth_token;

        j["mcp_servers"][d.name] = new_mcp;
        if (WriteConfigFile(std::move(j))) {
            HandleConfigFileUpdated();
        }

    } catch (const std::exception& e) {
        clERROR() << "Failed to add new SSE mcp server:" << e.what() << endl;
    }
}

void Manager::AddNewMcp(const llm::LocalMcp& d)
{
    try {
        ASSIGN_OPT_OR_RETURN(auto j, GetConfigAsJSON(), );
        if (!j.contains("mcp_servers")) {
            j["mcp_servers"] = {};
        }

        auto mcp_servers = j["mcp_servers"];
        if (mcp_servers.contains(d.name)) {
            mcp_servers.erase(d.name);
        }

        llm::json new_mcp;
        new_mcp["type"] = "stdio";
        new_mcp["enabled"] = true;
        new_mcp["command"] = d.command;
        new_mcp["env"] = d.env;

        j["mcp_servers"][d.name] = new_mcp;
        if (WriteConfigFile(std::move(j))) {
            HandleConfigFileUpdated();
        }

    } catch (const std::exception& e) {
        clERROR() << "Failed to add new local mcp server:" << e.what() << endl;
    }
}

bool Manager::WriteConfigFile(llm::json j)
{
    const WriteOptions opts{.ignore_workspace = true};
    const wxString filepath =
        FileManager::GetSettingFileFullPath(kAssistantConfigFile, WriteOptions{.ignore_workspace = true});
    if (!FileManager::WriteSettingsFileContent(filepath, wxString::FromUTF8(j.dump(2)), opts)) {
        clERROR() << "Failed to write configuration file:" << filepath << endl;
        return false;
    }
    clGetManager()->ReloadFile(filepath);
    return true;
}

/**
 * @brief Handles the file saved event and triggers configuration reload if the assistant config
 * file was saved.
 *
 * This function is called when a file is saved in the editor. It checks if the saved file is the
 * LLM assistant configuration file, and if so, triggers a configuration update by calling
 * HandleConfigFileUpdated(). If the saved file is not the config file, the function returns early.
 *
 * @param event the command event containing information about the file save operation
 */
void Manager::OnFileSaved(clCommandEvent& event)
{
    event.Skip();

    CHECK_PTR_RET(clGetManager()->GetActiveEditor());

    {
        // assistant.json file.?
        wxString llm_config_file =
            FileManager::GetSettingFileFullPath(kAssistantConfigFile, WriteOptions{.ignore_workspace = true});

        wxString filepath = clGetManager()->GetActiveEditor()->GetRemotePathOrLocal();
        if (filepath == llm_config_file) {
            HandleConfigFileUpdated();
        }
    }

    {
        // ~/.codelite/config/assistant-global-settings.json file.?
        wxFileName global_agent_config{llm::Config::GetFullPath()};

        wxString filepath = clGetManager()->GetActiveEditor()->GetRemotePathOrLocal();
        if (filepath == global_agent_config.GetFullPath()) {
            // Add validation
            llm::Config tmp_config;
            auto status = tmp_config.Load();
            if (!status.ok()) {
                ::clMessageBox(status.message(), "CodeLite", wxICON_WARNING | wxOK);
            } else {
                // New content is valid -> load the configuration
                HandleGlobalConfigFileUpdated();
            }
        }
    }
}

/**
 * @brief Handles configuration file update events by reloading the configuration and notifying
 * listeners.
 *
 * This method is called when the configuration file has been updated. It displays a busy cursor,
 * reloads the configuration from the LLM Manager, and if successful, creates and dispatches
 * a wxEVT_LLM_CONFIG_UPDATED event to notify other components of the configuration change.
 */
void Manager::HandleConfigFileUpdated()
{
    // Reload configuration
    wxBusyCursor bc{};
    if (!llm::Manager::GetInstance().ReloadConfig(std::nullopt, false)) {
        return;
    }

    clLLMEvent event_config_updates{wxEVT_LLM_CONFIG_UPDATED};
    event_config_updates.SetEventObject(this);
    AddPendingEvent(event_config_updates);
}

void Manager::HandleGlobalConfigFileUpdated()
{
    // Reload configuration
    wxBusyCursor bc{};
    GetConfig().Load();

    clLLMEvent event_config_updates{wxEVT_LLM_GLOBAL_CONFIG_UPDATED};
    event_config_updates.SetEventObject(this);
    AddPendingEvent(event_config_updates);
}

clStatus Manager::ValidateConfigFile(std::optional<wxString> content) const
{
    wxString config_content;
    if (content.has_value()) {
        config_content = content.value();
    } else {
        const WriteOptions opts{.ignore_workspace = true};
        auto file_content = FileManager::ReadSettingsFileContent(kAssistantConfigFile, opts);
        if (!file_content.has_value()) {
            return StatusNotFound();
        }
        config_content = file_content.value();
    }

    EnvSetter env;
    assistant::EnvExpander expander;
    auto expand_result = expander.ExpandWithResult(config_content.ToStdString(wxConvUTF8));
    if (!expand_result.IsSuccess()) {
        return StatusExpandError(wxString::FromUTF8(expand_result.GetErrorMessage()));
    }

    auto parse_result = assistant::ConfigBuilder::FromContent(expand_result.GetString());
    if (!parse_result.ok()) {
        return StatusParseError(wxString::FromUTF8(parse_result.errmsg_));
    }
    return StatusOk();
}

/**
 * @brief Creates or opens the assistant configuration file, ensuring it is valid and up-to-date.
 *
 * This function attempts to locate and validate the global assistant configuration file.
 * If the file doesn't exist, is invalid, or has an outdated version, it creates a backup
 * (if an old file exists) and generates a new configuration file with default settings.
 * The function performs version checking and ensures the configuration matches the expected format.
 *
 * @return clStatusOr<wxString> On success, returns the full path to the configuration file.
 *                              On failure, returns a status error with an appropriate message.
 */
clStatusOr<wxString> Manager::CreateOrOpenConfig()
{
    const WriteOptions opts{.ignore_workspace = true};
    wxString global_config_path = FileManager::GetSettingFileFullPath(kAssistantConfigFile, opts);
    wxString backup_file_path = global_config_path + ".old";

    auto validate_result = ValidateConfigFile();

    if (!validate_result.ok() && (StatusIsParseError(validate_result) || StatusIsNotFound(validate_result))) {
        // File not found or invalid JSON format.

        // Backup old file
        if (wxFileName::FileExists(global_config_path)) {
            wxLogNull nl;
            ::wxCopyFile(global_config_path, backup_file_path);
        }

        // Invalid file: could not parse it or missing - create a default file.
        if (!FileUtils::WriteFileContent(global_config_path, kDefaultSettings)) {
            wxString messgage;
            messgage << _("Failed to create configuration file:\n") << global_config_path;
            ::wxMessageBox(messgage, "CodeLite", wxICON_WARNING | wxOK | wxCENTER);
            return StatusIOError(messgage);
        }
        return global_config_path;
    }

    // A valid file was found - check its version.
    ASSIGN_OPT_OR_RETURN(auto j, GetConfigAsJSON(), StatusOther("Failed to parse file as JSON"));

    if (!j.contains(kConfigVersionProperty) || !j[kConfigVersionProperty].is_number_float() ||
        (j[kConfigVersionProperty].get<float>() != kConfigVersion)) {
        // Backup old file
        if (wxFileName::FileExists(global_config_path)) {
            wxLogNull nl;
            ::wxCopyFile(global_config_path, backup_file_path);
        }

        // An old version - create a default file.
        if (!FileUtils::WriteFileContent(global_config_path, kDefaultSettings)) {
            wxString messgage;
            messgage << _("Failed to create configuration file:\n") << global_config_path;
            ::wxMessageBox(messgage, "CodeLite", wxICON_WARNING | wxOK | wxCENTER);
            return StatusIOError(messgage);
        }
    }
    return global_config_path;
}

void Manager::OpenSettingsFileInEditor()
{
    auto res = CreateOrOpenConfig();
    if (!res.ok()) {
        clERROR() << res.error_message() << endl;
        return;
    }

    clGetManager()->OpenFile(res.value());
}

void Manager::EnableAllFunctions(bool b)
{
    if (!m_client) {
        clWARNING() << "Failed to call EnableAllFunctions(" << b << "). Null client" << endl;
        return;
    }
    clDEBUG() << "Successfully" << (b ? "enabled" : "disabled") << "all functions" << endl;
    m_client->GetFunctionTable().EnableAll(b);
}

void Manager::EnableFunctionByName(const wxString& name, bool b)
{
    if (!m_client) {
        clWARNING() << "Failed to call EnableFunctionByName(" << name << ", " << b << "). Null client" << endl;
        return;
    }
    m_client->GetFunctionTable().EnableFunction(name.ToStdString(wxConvUTF8), b);
}

void Manager::ShowChatWindow(const wxString& prompt) { m_chatAI->ShowChatWindow(prompt); }

void Manager::ShowTextGenerationDialog(const wxString& prompt,
                                       std::optional<assistant::ChatOptions> chat_options,
                                       PreviewKind preview_kind,
                                       std::function<void()> completion_callback)
{
    CHECK_PTR_RET(m_commentGenerationView);
    assistant::ChatOptions opts{assistant::ChatOptions::kNoTools};
    assistant::AddFlagSet(opts, assistant::ChatOptions::kNoHistory);

    if (chat_options.has_value()) {
        opts = std::move(chat_options.value());
    }

    m_commentGenerationView->InitialiseFor(preview_kind);
    m_commentGenerationView->Show();
    m_commentGenerationView->StartProgress(_("Working..."));

    auto collector = new llm::ResponseCollector();
    collector->SetStateChangingCB([this](llm::ChatState state) {
        if (!wxThread::IsMain()) {
            clWARNING() << "StateChangingCB called for non main thread!" << endl;
            return;
        }
        switch (state) {
        case llm::ChatState::kThinking:
            m_commentGenerationView->UpdateProgress(_("Thinking..."));
            break;
        case llm::ChatState::kWorking:
            m_commentGenerationView->UpdateProgress(_("Working..."));
            break;
        case llm::ChatState::kReady:
            m_commentGenerationView->StopProgress(_("Ready"));
            break;
        }
    });

    collector->SetStreamCallback([this, completion_callback = std::move(completion_callback)](
                                     const std::string& message, llm::StreamCallbackReason reason) {
        if (llm::IsFlagSet(reason, llm::StreamCallbackReason::kCancelled)) {
            if (completion_callback) {
                completion_callback();
            }
            return;
        }

        bool is_done = llm::IsFlagSet(reason, llm::StreamCallbackReason::kDone);

        m_commentGenerationView->AppendText(wxString::FromUTF8(message));
        if (is_done && !m_commentGenerationView->IsShown()) {
            m_commentGenerationView->Show();
        }

        if (is_done && completion_callback) {
            completion_callback();
        }
    });

    Chat(collector, prompt, nullptr, opts);
}

const std::vector<wxString>& Manager::GetAvailablePlaceHolders() const { return kPlaceHolders; }

CanInvokeToolResult Manager::PromptUserYesNoTrustQuestion(const wxString& text, std::function<void()> on_trust_cb)
{
    const wxString kTrust = wxT("⟪t⟫");
    const wxString kTriplet = wxT("⟪y/n/t⟫");
    wxString message;
    message << text;
    if (!text.EndsWith("\n")) {
        message << "\n";
    }

    wxString question_message;
    question_message << _("Allow this action? use ") << kTrust
                     << _(" to trust (always allow this tool for the session). ") << kTriplet;
    message << question_message;
    auto fut = GetInstance().PromptUser(message, IconType::kNoIcon);

    // Avoid blocking the worker thread forever
    auto& mgr = GetInstance();
    std::string res;
    while (fut && fut->wait_for(std::chrono::milliseconds(10)) != std::future_status::ready) {
        if (mgr.IsClientStopping()) {
            return CanInvokeToolResult{
                .can_invoke = false,
                .reason = "Permission request aborted during shutdown/restart",
            };
        }
    }
    res = fut->get();
    wxString response_text = wxString::FromUTF8(res);
    response_text.Trim().Trim(false);

    if (response_text == "yes" || response_text == "y" || response_text == "ok") {
        return CanInvokeToolResult{
            .can_invoke = true,
        };
    } else if (response_text == "no" || response_text == "n") {
        return CanInvokeToolResult{
            .can_invoke = false,
            .reason = "Permission denied",
        };
    } else if (response_text == "trust" || response_text == "t") {
        if (on_trust_cb) {
            EventNotifier::Get()->RunOnMain<void>(std::move(on_trust_cb));
        }
        return CanInvokeToolResult{
            .can_invoke = true,
        };
    } else {
        // Something else - send the text as it is.
        return CanInvokeToolResult{
            .can_invoke = false,
            .reason = res,
        };
    }
}

void Manager::SetCachingPolicy(llm::CachePolicy policy)
{
    CHECK_PTR_RET(m_client);
    m_client->SetCachingPolicy(policy);
}

Manager::PromptFuture Manager::PromptUser(const wxString& msg, IconType icon)
{
    CHECK_PTR_RET_NULL(m_client);
    CHECK_PTR_RET_NULL(GetChatWindow());

    auto cb = [msg, icon, this]() -> Manager::PromptFuture {
        auto chat_win = GetChatWindow();
        wxString symbol = IconType_ToString(icon);
        if (!symbol.empty()) {
            symbol << " ";
        }

        wxString current_text = chat_win->GetText();
        wxString message_to_add;
        if (!current_text.EndsWith("\n")) {
            message_to_add = "\n";
        }
        message_to_add << symbol << msg;
        if (!msg.EndsWith("\n")) {
            message_to_add << "\n";
        }
        chat_win->AppendText(message_to_add);
        // Switch the focus to the answer window.
        chat_win->GetStcInput()->CallAfter(&wxStyledTextCtrl::SetFocus);
        auto response_promise = std::make_shared<std::promise<std::string>>();
        auto fut = response_promise->get_future();
        chat_win->PushPromise(response_promise);
        return std::make_shared<std::future<std::string>>(std::move(fut));
    };
    return EventNotifier::Get()->RunOnMain<Manager::PromptFuture>(std::move(cb));
}

void Manager::PrintMessage(const wxString& msg, IconType icon)
{
    CHECK_PTR_RET(m_client);
    CHECK_PTR_RET(GetChatWindow());

    auto cb = [msg, icon, this]() {
        auto chat_win = GetChatWindow();
        wxString symbol = IconType_ToString(icon) + " ";

        wxString current_text = chat_win->GetText();
        wxString message_to_add;
        if (!current_text.EndsWith("\n")) {
            message_to_add = "\n";
        }
        message_to_add << symbol << msg;
        if (!msg.EndsWith("\n")) {
            message_to_add << "\n";
        }
        chat_win->AppendText(message_to_add);
    };
    EventNotifier::Get()->RunOnMain<void>(std::move(cb));
}

std::shared_ptr<std::atomic_bool> Manager::NewTerminationFlag()
{
    std::lock_guard lk{m_mutex};
    std::shared_ptr<std::atomic_bool> flag = std::make_shared<std::atomic_bool>(false);
    m_termination_flags.push_back(flag);
    return flag;
}

void Manager::DeleteTerminationFlag(std::shared_ptr<std::atomic_bool> flag)
{
    std::lock_guard lk{m_mutex};
    std::erase_if(
        m_termination_flags, [flag](std::shared_ptr<std::atomic_bool> f) -> bool { return flag.get() == f.get(); });
}

void Manager::OnGenerateDocString(wxCommandEvent& event)
{
    wxUnusedVar(event);

    IEditor* editor = clGetManager()->GetActiveEditor();
    CHECK_PTR_RET(editor);
    CHECK_PTR_RET(m_commentGenerationView);

    auto stc = editor->GetCtrl();
    auto res = CTags::ParseBufferSymbols(editor->GetRemotePathOrLocal(), stc->GetText());
    if (!res.ok()) {
        ::clMessageBox(res.error_message(), "CodeLite", wxOK | wxICON_WARNING);
        return;
    }

    auto symbols = res.value();
    if (symbols.empty()) {
        clDEBUG() << "No symbols found for file:" << editor->GetRemotePathOrLocal() << endl;
        return;
    }

    auto symbol_range = CTags::FindSymbolsRangeNearLine(symbols, editor->GetCurrentLine() + 1);
    if (!symbol_range.has_value()) {
        return;
    }

    int start_line = symbol_range.value().start_line - 1;
    int end_line = symbol_range.value().end_line.value_or(stc->GetLineCount()) - 1;

    wxString func_text;
    if (start_line == end_line) {
        // take the entire line
        func_text = stc->GetLine(start_line);
    } else {
        // Take everything between the start and end line.
        int start_pos = stc->PositionFromLine(start_line);
        int end_pos = stc->PositionFromLine(end_line) + stc->LineLength(end_line);
        func_text = stc->GetTextRange(start_pos, end_pos);
    }

    if (func_text.empty()) {
        ::clMessageBox(_("Unable to gather enough context to generate docstring"), "CodeLite", wxOK | wxICON_WARNING);
        return;
    }

    clGetManager()->SetStatusMessage(_("Generating DocString..."), 1);
    wxString prompt = GetConfig().GetPrompt(llm::kPromptGenerateComment);
    prompt.Replace("{{function}}", func_text);

    m_commentGenerationView->InitialiseFor(PreviewKind::kCommentGeneration);
    ShowTextGenerationDialog(prompt, std::nullopt, PreviewKind::kCommentGeneration);
}

void Manager::CompleteInitialisation()
{
    m_commentGenerationView = new TextGenerationPreviewFrame(PreviewKind::kCommentGeneration);
    m_commentGenerationView->Hide();

    clKeyboardManager::Get()->AddAccelerator(
        _("AI"),
        {{"lsp_document_scope", _("Generate an AI-powered comment for the current function"), "Ctrl-Shift-M"}});

    wxTheApp->Bind(wxEVT_MENU, &Manager::OnGenerateDocString, this, XRCID("lsp_document_scope"));
    EventNotifier::Get()->Bind(wxEVT_CONTEXT_MENU_EDITOR, &Manager::OnEditorContextMenu, this);
}

void Manager::OnWorkspaceOpened(clWorkspaceEvent& event) { event.Skip(); }
void Manager::OnWorkspaceClosed(clWorkspaceEvent& event) { event.Skip(); }

bool Manager::StoreCurrentConverstation(const wxString& conversation_text)
{
    auto ep = GetActiveEndpoint();
    if (!ep.has_value()) {
        return false;
    }
    auto c = NewConversation(conversation_text);
    if (!c.has_value()) {
        return false;
    }

    return GetHistoryStore().Put(ep.value(), c.value());
}

std::optional<std::pair<wxString, bool>>
Manager::ShowTrustLevelDialog(const wxString& toolname, const std::vector<std::pair<wxString, wxString>>& options)
{
    ToolTrustLevelDlg dlg(nullptr, options, 0);
    dlg.SetMessage(wxString::Format(_("Define the level of trust for: '%s'"), toolname));
    dlg.Check(false);
    dlg.SetAllowTrustEntireTool(true);

    if (dlg.ShowModal() != wxID_OK) {
        return std::nullopt;
    }

    auto result = dlg.GetValue();
    if (!result.has_value()) {
        return std::nullopt;
    }
    return std::make_pair(result.value(), dlg.IsChecked());
}

bool Manager::CheckIfPathIsAllowedForTool(const wxString& toolname, const wxString& path, PathMatch flags)
{
    return GetConfig().IsToolTrustedFor(toolname, [toolname, path, flags](const wxString& trusted_pattern) -> bool {
        wxString normalized_path = llm::IsFlagSet(flags, PathMatch::kIsPath) ? FileUtils::NormalizePath(path) : path;
        clDEBUG() << "Checking if tool:" << toolname << "with trusted pattern:" << trusted_pattern
                  << "is allowed for file: '" << normalized_path << "'";
        bool result{false};
        bool exact_match_only = llm::IsFlagSet(flags, PathMatch::kExactMatchOnly);

        if (!exact_match_only && trusted_pattern == "*") {
            // All
            clDEBUG() << "Allowed pattern is * => true" << endl;
            result = true;
        } else if (!exact_match_only && trusted_pattern.EndsWith("*")) {
            wxString dirprefix = trusted_pattern.Mid(0, trusted_pattern.size() - 1);
            // Prefix
            result = normalized_path.StartsWith(dirprefix);
            clDEBUG() << "checking if: '" << normalized_path << "' starts_with: '" << dirprefix << "'" << "=>" << result
                      << endl;
        } else {
            // Exact match
            result = (normalized_path == trusted_pattern);
            clDEBUG() << "Using exact match between '" << normalized_path << "' and '" << trusted_pattern << "'"
                      << "=>" << result << endl;
        }
        return result;
    });
}

bool Manager::CheckIfShellCommandAllowed(const wxString& toolname,
                                         const wxString& cmd,
                                         const wxString& working_directory)
{
    clDEBUG() << "Checking if command:" << cmd << "is allowed." << endl;

    // Split the command for the "main" command to check
    auto commands = StringUtils::SplitShellCommand(cmd);
    if (commands.empty()) {
        return false;
    }

    wxArrayString commands_to_check;
    for (const auto& v : commands) {
        if (v.empty())
            continue;
        commands_to_check.Add(v[0] + " "); // Need the extra space since we keep items as "ls *"
    }

    if (commands_to_check.empty()) {
        return false;
    }

    clDEBUG() << "Checking if the following commands:" << commands_to_check << "are allowed." << endl;
    for (const wxString& cmd_to_check : commands_to_check) {
        if (!GetInstance().CheckIfPathIsAllowedForTool(toolname, cmd_to_check)) {
            return false;
        }
    }
    // All commands are allowed.
    return true;
}
} // namespace llm
