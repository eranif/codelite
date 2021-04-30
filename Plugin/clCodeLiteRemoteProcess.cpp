#include "JSON.h"
#include "StringUtils.h"
#include "asyncprocess.h"
#include "cJSON.h"
#include "clCodeLiteRemoteProcess.hpp"
#include "clSFTPManager.hpp"
#include "cl_command_event.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "processreaderthread.h"
#include <functional>
#include <unordered_map>
#include <wx/event.h>
#include <wx/tokenzr.h>

wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_LIST_FILES, clCommandEvent);
wxDEFINE_EVENT(wxEVT_CODELITE_REMOTE_FIND_RESULTS, clFindInFilesEvent);

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

void clCodeLiteRemoteProcess::StartInteractive(const SSHAccountInfo& account, const wxString& scriptPath)
{
    if(m_process) {
        return;
    }

    // upload codelite-remote script and start it once its uploaded
    wxString localCodeLiteRemoteScript = clStandardPaths::Get().GetBinFolder() + "/codelite-remote";
    if(!clSFTPManager::Get().AwaitSaveFile(localCodeLiteRemoteScript, scriptPath, account.GetAccountName())) {
        clERROR() << "Failed to upload file:" << scriptPath << "." << clSFTPManager::Get().GetLastError() << endl;
        return;
    }

    // wrap the command in ssh
    vector<wxString> command = { "ssh", "-o", "ServerAliveInterval=10", "-o", "StrictHostKeyChecking=no" };
    command.push_back(account.GetUsername() + "@" + account.GetHost());
    command.push_back("-p");
    command.push_back(wxString() << account.GetPort());

    // start the process in interactive mode
    command.push_back("python3 " + scriptPath + " --i");

    // start the process
    m_process = ::CreateAsyncProcess(this, command, IProcessStderrEvent | IProcessCreateDefault);
}

void clCodeLiteRemoteProcess::StartSync(const SSHAccountInfo& account, const wxString& scriptPath,
                                        const std::vector<wxString>& args, wxString& output)
{
    // wrap the command in ssh
    vector<wxString> command = { "ssh", "-o", "ServerAliveInterval=10", "-o", "StrictHostKeyChecking=no" };
    command.push_back(account.GetUsername() + "@" + account.GetHost());
    command.push_back("-p");
    command.push_back(wxString() << account.GetPort());

    // start the process in interactive mode
    wxString script_argv;
    for(const auto& arg : args) {
        script_argv << arg << " ";
    }

    if(script_argv.empty()) {
        script_argv.RemoveLast();
    }
    command.push_back("python3 " + scriptPath + " \"" << script_argv << "\"");

    wxString cmdline;
    for(const wxString& arg : command) {
        cmdline << arg << " ";
    }
    cmdline.RemoveLast();
    clDEBUG() << "Running:" << cmdline << endl;

    // start the process
    IProcess::Ptr_t proc(::CreateSyncProcess(cmdline));
    proc->WaitForTerminate(output);
}

void clCodeLiteRemoteProcess::OnProcessOutput(clProcessEvent& e)
{
    m_outputRead << e.GetOutput();
    ProcessOutput();
}

void clCodeLiteRemoteProcess::OnProcessTerminated(clProcessEvent& e)
{
    if(m_terminateCallback) {
        m_terminateCallback();
    }
    Cleanup();
}

void clCodeLiteRemoteProcess::Stop()
{
    if(m_process) {
        m_process->Write(wxString("exit\n"));
    }
    wxDELETE(m_process);
    Cleanup();
}

void clCodeLiteRemoteProcess::SetTerminateCallback(std::function<void()> callback)
{
    m_terminateCallback = std::move(callback);
}

void clCodeLiteRemoteProcess::Cleanup()
{
    while(!m_completionCallbacks.empty()) {
        m_completionCallbacks.pop_back();
    }
    wxDELETE(m_process);
    m_terminateCallback = nullptr;
}

void clCodeLiteRemoteProcess::ProcessOutput()
{
    size_t where = m_outputRead.find('\n');
    while(where != wxString::npos) {
        wxString msg = m_outputRead.Mid(0, where);
        m_outputRead.erase(0, where + 1); // remove the consumed line + the "\n"
        if(m_completionCallbacks.empty()) {
            where = m_outputRead.find('\n');
            continue;
        }

        auto cb = m_completionCallbacks.front();
        (this->*cb)(msg);

        m_completionCallbacks.pop_front();
        where = m_outputRead.find('\n');
    }
}

void clCodeLiteRemoteProcess::OnListFilesCompleted(const wxString& output)
{
    clCommandEvent event(wxEVT_CODELITE_REMOTE_LIST_FILES);

    // parse the output
    JSON root(output);
    if(!root.isOk()) {
        return;
    }

    wxArrayString files = root.toElement().toArrayString();
    event.GetStrings().swap(files);
    AddPendingEvent(event);
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
    m_completionCallbacks.push_back(&clCodeLiteRemoteProcess::OnListFilesCompleted);
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
    clDEBUG() << command << endl;

    // push a callback
    m_completionCallbacks.push_back(&clCodeLiteRemoteProcess::OnFindCompleted);
}

void clCodeLiteRemoteProcess::OnFindCompleted(const wxString& output)
{
    clDEBUG() << output << endl;
    JSON root(output);
    if(!root.isOk()) {
        return;
    }

    auto arr = root.toElement();

    size_t arr_size = arr.arraySize();
    clFindInFilesEvent::Match::vec_t matches;
    matches.reserve(arr_size);

    for(size_t i = 0; i < arr_size; ++i) {
        auto file_matches = arr[i];
        clFindInFilesEvent::Match match;
        match.file = file_matches["file"].toString();
        auto json_locations = file_matches["matches"];
        size_t loc_count = json_locations.arraySize();

        // add the locations for this file
        match.locations.reserve(loc_count);
        for(size_t j = 0; j < loc_count; j++) {
            clFindInFilesEvent::Location loc;
            auto json_loc = json_locations[j];
            loc.line = json_loc["ln"].toSize_t();
            loc.column_start = json_loc["start"].toSize_t();
            loc.column_end = json_loc["end"].toSize_t();
            loc.pattern = json_loc["pattern"].toString();
            match.locations.emplace_back(loc);
        }

        // add the match
        matches.emplace_back(match);
    }

    clFindInFilesEvent event(wxEVT_CODELITE_REMOTE_FIND_RESULTS);
    event.SetMatches(matches);
    AddPendingEvent(event);
}
