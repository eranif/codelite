#include "clCodeLiteRemoteProcess.hpp"

#include "JSON.h"
#include "StringUtils.h"
#include "asyncprocess.h"
#include "cJSON.h"
#include "clSFTPManager.hpp"
#include "cl_command_event.h"
#include "environmentconfig.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "processreaderthread.h"

#include <functional>
#include <unordered_map>
#include <wx/event.h>
#include <wx/tokenzr.h>

wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_RESTARTED, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_LIST_FILES, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_LIST_FILES_DONE, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_FIND_RESULTS, clFindInFilesEvent);
wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_FIND_RESULTS_DONE, clFindInFilesEvent);
wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_EXEC_OUTPUT, clProcessEvent);
wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_EXEC_DONE, clProcessEvent);
wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_LOCATE_DONE, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_LOCATE, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_FINDPATH, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_FINDPATH_DONE, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_LIST_LSPS, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_LIST_LSPS_DONE, clCommandEvent);
namespace
{
const char msg_terminator[] = ">>codelite-remote-msg-end<<\n";
size_t msg_terminator_len = sizeof(msg_terminator) - 1;
} // namespace

namespace
{
class CodeLiteRemoteProcess : public IProcess
{
    clCodeLiteRemoteProcess* m_process = nullptr;
    std::function<void(const wxString&)> m_callback = nullptr;
    wxString m_output;

private:
    bool DoWrite(const wxString& buff)
    {
        if(!m_process) {
            return false;
        }
        m_process->Write(buff);
        return true;
    }

public:
    CodeLiteRemoteProcess(wxEvtHandler* handler, clCodeLiteRemoteProcess* process)
        : IProcess(handler)
        , m_process(process)
    {
    }
    ~CodeLiteRemoteProcess() { m_process = nullptr; }

    // are we using callback?
    bool IsUsingCallback() const { return m_callback != nullptr; }

    // Use callback instead of events
    void SetCallback(std::function<void(const wxString&)> cb) { m_callback = std::move(cb); }

    // Stop notifying the parent window about input/output from the process
    // this is useful when we wish to terminate the process onExit but we don't want
    // to know about its termination
    void Detach() override {}

    // Read from process stdout - return immediately if no data is available
    bool Read(wxString& buff, wxString& buffErr) override { return false; }

    // Write to the process stdin
    // This version add LF to the buffer
    bool Write(const wxString& buff) override { return DoWrite(buff); }

    // ANSI version
    // This version add LF to the buffer
    bool Write(const std::string& buff) override { return DoWrite(buff); }

    // Write to the process stdin
    bool WriteRaw(const wxString& buff) override { return DoWrite(buff); }

    // ANSI version
    bool WriteRaw(const std::string& buff) override { return DoWrite(buff); }

    /**
     * @brief this method is mostly needed on MSW where writing a password
     * is done directly on the console buffer rather than its stdin
     */
    bool WriteToConsole(const wxString& buff) override { return DoWrite(buff + "\n"); }

    // Return true if the process is still alive
    bool IsAlive() override { return m_process && m_process->IsRunning(); }

    // Clean the process resources and kill the process if it is
    // still alive
    void Cleanup() override {}

    // Terminate the process. It is recommended to use this method
    // so it will invoke the 'Cleaup' procedure and the process
    // termination event will be sent out
    void Terminate() override {}

    /**
     * @brief send signal to the process
     */
    void Signal(wxSignal sig) override { wxUnusedVar(sig); }

    void PostOutputEvent(const wxString& output)
    {
        if(m_callback) {
            // when callback is used, we aggregate the output
            // and post it in the terminated event
            m_output << output;
        } else {
            clProcessEvent e(wxEVT_ASYNC_PROCESS_OUTPUT);
            e.SetOutput(output);
            e.SetProcess(this);
            m_parent->AddPendingEvent(e);
        }
    }

    void PostTerminateEvent()
    {
        if(m_callback) {
            m_callback(m_output);
        } else {
            clProcessEvent e(wxEVT_ASYNC_PROCESS_TERMINATED);
            e.SetProcess(this);
            m_parent->AddPendingEvent(e);
        }
    }
};
} // namespace

clCodeLiteRemoteProcess::clCodeLiteRemoteProcess()
{
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clCodeLiteRemoteProcess::OnProcessTerminated, this);
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clCodeLiteRemoteProcess::OnProcessOutput, this);
}

clCodeLiteRemoteProcess::~clCodeLiteRemoteProcess()
{
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &clCodeLiteRemoteProcess::OnProcessTerminated, this);
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &clCodeLiteRemoteProcess::OnProcessOutput, this);
    wxDELETE(m_process);
}

void clCodeLiteRemoteProcess::StartInteractive(const wxString& account, const wxString& scriptPath,
                                               const wxString& contextString)
{
    auto ssh_account = SSHAccountInfo::LoadAccount(account);
    if(ssh_account.GetAccountName().empty()) {
        clWARNING() << "Failed to load ssh account:" << account << endl;
        return;
    }
    StartInteractive(ssh_account, scriptPath, contextString);
}

void clCodeLiteRemoteProcess::StartIfNotRunning()
{
    if(IsRunning()) {
        return;
    }

    // wrap the command in ssh
    wxFileName ssh_exe;
    EnvSetter setter;
    if(!FileUtils::FindExe("ssh", ssh_exe)) {
        clERROR() << "Could not locate ssh executable in your PATH!" << endl;
        return;
    }

    vector<wxString> command = { ssh_exe.GetFullPath(), "-o", "ServerAliveInterval=10", "-o",
                                 "StrictHostKeyChecking=no" };
    command.push_back(m_account.GetUsername() + "@" + m_account.GetHost());
    command.push_back("-p");
    command.push_back(wxString() << m_account.GetPort());

    // start the process in interactive mode
    command.push_back("python3 " + m_scriptPath + " --context " + GetContext());

    clDEBUG() << "Starting codelite-remote:" << command << endl;
    // start the process
    m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault | IProcessRawOutput);
}

void clCodeLiteRemoteProcess::StartInteractive(const SSHAccountInfo& account, const wxString& scriptPath,
                                               const wxString& contextString)
{
    if(m_process) {
        return;
    }

#if USE_SFTP
    // upload codelite-remote script and start it once its uploaded
    wxString localCodeLiteRemoteScript = clStandardPaths::Get().GetBinFolder() + "/codelite-remote";
    if(!clSFTPManager::Get().AwaitSaveFile(localCodeLiteRemoteScript, scriptPath, account.GetAccountName())) {
        clERROR() << "Failed to upload file:" << scriptPath << "." << clSFTPManager::Get().GetLastError() << endl;
        return;
    }
#else
    clERROR() << "CodeLite is build with NO SFTP support" << endl;
#endif

    m_going_down = false;
    m_context = contextString;
    m_account = account;
    m_scriptPath = scriptPath;

    StartIfNotRunning();
}

void clCodeLiteRemoteProcess::OnProcessOutput(clProcessEvent& e)
{
    m_outputRead << e.GetOutput();
    ProcessOutput();
}

void clCodeLiteRemoteProcess::OnProcessTerminated(clProcessEvent& e)
{
    wxUnusedVar(e);
    Cleanup();
    if(!m_going_down) {
        // restart the process
        StartIfNotRunning();

        clCommandEvent terminate_event(wxEVT_CODELITE_REMOTE_RESTARTED);
        terminate_event.SetEventObject(this);
        AddPendingEvent(terminate_event);
    }
    m_going_down = false;
}

void clCodeLiteRemoteProcess::Stop()
{
    m_going_down = true;
    if(m_process) {
        m_process->Write(wxString("exit\n"));
    }
    wxDELETE(m_process);
    Cleanup();
}

void clCodeLiteRemoteProcess::Cleanup()
{
    while(!m_completionCallbacks.empty()) {
        m_completionCallbacks.pop_back();
    }
    wxDELETE(m_process);
}

bool clCodeLiteRemoteProcess::GetNextBuffer(wxString& raw_input_buffer, wxString& buffer, bool& is_completed)
{
    size_t separator_len = msg_terminator_len;
    size_t where = raw_input_buffer.find(msg_terminator);
    if(where == wxString::npos) {
        // locate the \n from the end
        is_completed = false;
        where = raw_input_buffer.rfind("\n");
        separator_len = 1;
    } else {
        is_completed = true;
    }

    if(where != wxString::npos) {
        size_t length_to_take = where;
        if(separator_len == 1) {
            length_to_take += 1;
        }
        buffer = raw_input_buffer.Mid(0, length_to_take);
        raw_input_buffer.erase(0, where + separator_len);
    }
    return where != wxString::npos;
}

void clCodeLiteRemoteProcess::ProcessOutput()
{
    bool is_completed = false;
    wxString buffer;

    while(GetNextBuffer(m_outputRead, buffer, is_completed)) {
        if(m_completionCallbacks.empty()) {
            clDEBUG() << "Read: [" << buffer << "]. But there are no completion callback" << endl;
            continue;
        }

        auto p = m_completionCallbacks.front();
        if(p.handler) {
            auto handler = static_cast<CodeLiteRemoteProcess*>(p.handler);
            handler->PostOutputEvent(buffer);
            if(is_completed) {
                handler->PostTerminateEvent();

                // when using callback the handler is handled internally
                if(handler->IsUsingCallback()) {
                    delete handler;
                }
            }
        } else if(p.func) {
            (this->*p.func)(buffer, is_completed);
        }

        if(is_completed) {
            m_completionCallbacks.pop_front();
            ResetStates();
        }
    }
}

void clCodeLiteRemoteProcess::ListLSPs()
{
    if(!m_process) {
        return;
    }

    // build the command and send it
    JSON root(cJSON_Object);
    auto item = root.toElement();
    item.addProperty("command", "list_lsps");
    m_process->Write(item.format(false) + "\n");

    // push a callback
    m_completionCallbacks.push_back({ &clCodeLiteRemoteProcess::OnListLSPsOutput, nullptr });
}

void clCodeLiteRemoteProcess::ListFiles(const wxString& root_dir, const wxString& extensions)
{
    if(!m_process) {
        return;
    }

    wxString exts = extensions;
    exts.Replace("*", "");

    // build the command and send it
    JSON root(cJSON_Object);
    auto item = root.toElement();
    item.addProperty("command", "ls");
    item.addProperty("root_dir", root_dir);
    item.addProperty("file_extensions", ::wxStringTokenize(exts, ",; |", wxTOKEN_STRTOK));
    m_process->Write(item.format(false) + "\n");

    // push a callback
    m_completionCallbacks.push_back({ &clCodeLiteRemoteProcess::OnListFilesOutput, nullptr });
}

void clCodeLiteRemoteProcess::Search(const wxString& root_dir, const wxString& extensions, const wxString& find_what,
                                     bool whole_word, bool icase)
{
    if(!m_process) {
        return;
    }

    wxString exts = extensions;
    exts.Replace("*", "");

    // build the command and send it
    JSON root(cJSON_Object);
    auto item = root.toElement();
    item.addProperty("command", "find");
    item.addProperty("root_dir", root_dir);
    item.addProperty("find_what", find_what);
    item.addProperty("file_extensions", ::wxStringTokenize(exts, ",; |", wxTOKEN_STRTOK));
    item.addProperty("icase", icase);
    item.addProperty("whole_word", whole_word);

    wxString command = item.format(false);
    m_process->Write(command + "\n");
    clDEBUG1() << command << endl;

    // push a callback
    m_completionCallbacks.push_back({ &clCodeLiteRemoteProcess::OnFindOutput, nullptr });
}

void clCodeLiteRemoteProcess::Locate(const wxString& path, const wxString& name, const wxString& ext,
                                     const std::vector<wxString>& versions)
{
    if(!m_process) {
        return;
    }

    // build the command and send it
    JSON root(cJSON_Object);
    auto item = root.toElement();
    item.addProperty("command", "locate");
    item.addProperty("path", path);
    item.addProperty("name", name);
    item.addProperty("ext", ext);

    // convert std::vector to wxArrayString
    wxArrayString v;
    v.reserve(versions.size());

    for(const auto& s : versions) {
        v.Add(s);
    }

    item.addProperty("versions", v);

    wxString command = item.format(false);
    m_process->Write(command + "\n");
    clDEBUG1() << command << endl;

    // push a callback
    m_completionCallbacks.push_back({ &clCodeLiteRemoteProcess::OnLocateOutput, nullptr });
}

void clCodeLiteRemoteProcess::FindPath(const wxString& path)
{
    if(!m_process) {
        return;
    }

    // build the command and send it
    JSON root(cJSON_Object);
    auto item = root.toElement();
    item.addProperty("command", "find_path");
    item.addProperty("path", path);

    wxString command = item.format(false);
    m_process->Write(command + "\n");
    clDEBUG1() << command << endl;

    // push a callback
    m_completionCallbacks.push_back({ &clCodeLiteRemoteProcess::OnFindPathOutput, nullptr });
}

void clCodeLiteRemoteProcess::ResetStates()
{
    m_fif_matches_count = 0;
    m_fif_files_scanned = 0;
}

bool clCodeLiteRemoteProcess::DoExec(const wxString& cmd, const wxString& working_directory, const clEnvList_t& env,
                                     IProcess* handler)
{
    if(!m_process) {
        return false;
    }

    // build the command and send it
    JSON root(cJSON_Object);
    auto item = root.toElement();
    item.addProperty("command", "exec");
    item.addProperty("wd", working_directory);
    item.addProperty("cmd", cmd);

    auto envarr = item.AddArray("env");
    for(const auto& p : env) {
        auto entry = envarr.AddObject(wxEmptyString);
        entry.addProperty("name", p.first);
        entry.addProperty("value", p.second);
    }

    wxString command = item.format(false);
    m_process->Write(command + "\n");
    clDEBUG1() << command << endl;

    // push a callback
    m_completionCallbacks.push_back({ &clCodeLiteRemoteProcess::OnExecOutput, handler });
    return true;
}

void clCodeLiteRemoteProcess::Exec(const wxArrayString& args, const wxString& working_directory, const clEnvList_t& env)
{
    wxString cmdstr = GetCmdString(args);
    if(cmdstr.empty()) {
        return;
    }
    DoExec(cmdstr, working_directory, env);
}

void clCodeLiteRemoteProcess::Exec(const wxString& cmd, const wxString& working_directory, const clEnvList_t& env)
{
    DoExec(cmd, working_directory, env);
}

void clCodeLiteRemoteProcess::Write(const wxString& str)
{
    if(IsRunning()) {
        return;
    }
    if(!str.EndsWith("\n")) {
        m_process->Write(str + "\n");
    } else {
        m_process->Write(str);
    }
}

IProcess* clCodeLiteRemoteProcess::CreateAsyncProcess(wxEvtHandler* handler, const wxString& cmd,
                                                      const wxString& working_directory, const clEnvList_t& env)
{
    CodeLiteRemoteProcess* p = new CodeLiteRemoteProcess(handler, this);
    if(DoExec(cmd, working_directory, env, p)) {
        return p;
    }
    wxDELETE(p);
    return nullptr;
}

void clCodeLiteRemoteProcess::CreateAsyncProcessCB(const wxString& cmd, std::function<void(const wxString&)> callback,
                                                   const wxString& working_directory, const clEnvList_t& env)
{
    CodeLiteRemoteProcess* p = new CodeLiteRemoteProcess(nullptr, this);
    p->SetCallback(std::move(callback));
    if(DoExec(cmd, working_directory, env, p)) {
        return;
    }
    wxDELETE(p);
}

// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------
// completion handlers
// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------

void clCodeLiteRemoteProcess::OnListLSPsOutput(const wxString& output, bool is_completed)
{
    clCommandEvent event(wxEVT_CODELITE_REMOTE_LIST_LSPS);

    // parse the output
    wxArrayString lsps = ::wxStringTokenize(output, "\r\n", wxTOKEN_STRTOK);
    event.GetStrings().swap(lsps);
    AddPendingEvent(event);

    if(is_completed) {
        clCommandEvent event_done(wxEVT_CODELITE_REMOTE_LIST_LSPS_DONE);
        AddPendingEvent(event_done);
    }
}

void clCodeLiteRemoteProcess::OnListFilesOutput(const wxString& output, bool is_completed)
{
    clCommandEvent event(wxEVT_CODELITE_REMOTE_LIST_FILES);

    // parse the output
    wxArrayString files = ::wxStringTokenize(output, "\r\n", wxTOKEN_STRTOK);
    event.GetStrings().swap(files);
    AddPendingEvent(event);

    if(is_completed) {
        clCommandEvent event_done(wxEVT_CODELITE_REMOTE_LIST_FILES_DONE);
        AddPendingEvent(event_done);
    }
}

void clCodeLiteRemoteProcess::OnFindPathOutput(const wxString& output, bool is_completed)
{
    clCommandEvent event(wxEVT_CODELITE_REMOTE_FINDPATH);

    // parse the output
    clDEBUG1() << "FindPath output: [" << output << "]" << endl;
    wxString fullpath = output;
    fullpath.Trim().Trim(false);
    event.SetString(fullpath);
    AddPendingEvent(event);

    if(is_completed) {
        clCommandEvent event_done(wxEVT_CODELITE_REMOTE_FINDPATH_DONE);
        AddPendingEvent(event_done);
    }
}

void clCodeLiteRemoteProcess::OnLocateOutput(const wxString& output, bool is_completed)
{
    clCommandEvent event(wxEVT_CODELITE_REMOTE_LOCATE);

    // parse the output
    clDEBUG1() << "Locate output: [" << output << "]" << endl;
    wxString fullpath = output;
    fullpath.Trim().Trim(false);
    event.SetFileName(fullpath);
    AddPendingEvent(event);

    if(is_completed) {
        clCommandEvent event_done(wxEVT_CODELITE_REMOTE_LOCATE_DONE);
        AddPendingEvent(event_done);
    }
}

void clCodeLiteRemoteProcess::OnFindOutput(const wxString& output, bool is_completed)
{
    wxArrayString files = ::wxStringTokenize(output, "\r\n", wxTOKEN_STRTOK);
    if(files.empty()) {
        return;
    }

    clFindInFilesEvent::Match::vec_t matches;
    matches.reserve(files.size());
    for(const wxString& file : files) {
        JSON j(file);
        if(!j.isOk()) {
            continue;
        }

        auto file_matches = j.toElement();

        if((m_fif_matches_count == 0) && (m_fif_files_scanned == 0) && file_matches.hasNamedObject("files_scanned")) {
            m_fif_files_scanned = file_matches["files_scanned"].toSize_t();
            continue;
        }

        clFindInFilesEvent::Match match;
        match.file = file_matches["file"].toString();
        auto json_locations = file_matches["matches"];
        size_t loc_count = json_locations.arraySize();

        // add the locations for this file
        if(loc_count) {
            match.locations.reserve(loc_count);
            for(size_t j = 0; j < loc_count; j++) {
                clFindInFilesEvent::Location loc;
                auto json_loc = json_locations[j];
                loc.line = json_loc["ln"].toSize_t();
                loc.column_start = json_loc["start"].toSize_t();
                loc.column_end = json_loc["end"].toSize_t();
                loc.pattern = json_loc["pattern"].toString();
                match.locations.push_back(loc);
                m_fif_matches_count++;
            }
            // add the match
            matches.push_back(match);
        }
    }

    if(!matches.empty()) {
        clFindInFilesEvent event(wxEVT_CODELITE_REMOTE_FIND_RESULTS);
        event.SetMatches(matches);
        AddPendingEvent(event);
    }

    if(is_completed) {
        clFindInFilesEvent event_done(wxEVT_CODELITE_REMOTE_FIND_RESULTS_DONE);
        event_done.SetInt(m_fif_files_scanned);
        AddPendingEvent(event_done);
    }
}

void clCodeLiteRemoteProcess::OnExecOutput(const wxString& buffer, bool is_completed)
{
    if(!buffer.empty()) {
        clProcessEvent output_event(wxEVT_CODELITE_REMOTE_EXEC_OUTPUT);
        output_event.SetOutput(buffer);
        AddPendingEvent(output_event);
    }

    if(is_completed) {
        clProcessEvent end_event(wxEVT_CODELITE_REMOTE_EXEC_DONE);
        AddPendingEvent(end_event);
    }
}

bool clCodeLiteRemoteProcess::SyncExec(const wxString& cmd, const wxString& working_directory, const clEnvList_t& env,
                                       wxString* output)
{
    if(!m_completionCallbacks.empty()) {
        clWARNING() << "unable to run SyncExec() for command:" << cmd << "async queue is not empty" << endl;
        return false;
    }
    if(!m_process) {
        clWARNING() << "unable to run SyncExec() for command:" << cmd << "no process" << endl;
        return false;
    }
    // disable the background reader thread
    m_process->SuspendAsyncReads();

    if(!DoExec(cmd, working_directory, env)) {
        return false;
    }

    // DoExec pushes a callback to the queue - pop it
    // as we dont really need it
    m_completionCallbacks.pop_back();

    // read
    wxString buff_out, buff_err;
    m_outputRead.clear();

    wxString complete_output;
    while(m_process->Read(buff_out, buff_err)) {
        m_outputRead << buff_out;
        size_t where = m_outputRead.find(msg_terminator);
        if(where == wxString::npos) {
            continue;
        }

        // strip the terminator and make it our output
        *output = m_outputRead.Mid(0, where);
        clDEBUG1() << "SyncExec(" << cmd << "):" << *output << endl;

        m_outputRead.clear();
        // resume the async nature of the process
        m_process->ResumeAsyncReads();
        return true;
    }

    // process terminated
    clProcessEvent dummy;
    OnProcessTerminated(dummy);
    return false;
}
