#include "CodeLiteRemoteHelper.hpp"

#include "Platform.hpp"
#include "StringUtils.h"
#include "clWorkspaceManager.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"
#include "macromanager.h"

#if USE_SFTP
#include "clSFTPManager.hpp"
#endif

thread_local wxString m_codeliteRemoteJSONContent;

CodeLiteRemoteHelper::CodeLiteRemoteHelper()
{
    Bind(wxEVT_WORKSPACE_LOADED, &CodeLiteRemoteHelper::OnWorkspaceLoaded, this);
    Bind(wxEVT_WORKSPACE_CLOSED, &CodeLiteRemoteHelper::OnWorkspaceClosed, this);
    if(ThePlatform->Which("ssh", &m_ssh_exe)) {
        ::WrapWithQuotes(m_ssh_exe);
    }
}

CodeLiteRemoteHelper::~CodeLiteRemoteHelper()
{
    Unbind(wxEVT_WORKSPACE_LOADED, &CodeLiteRemoteHelper::OnWorkspaceLoaded, this);
    Unbind(wxEVT_WORKSPACE_CLOSED, &CodeLiteRemoteHelper::OnWorkspaceClosed, this);
}

void CodeLiteRemoteHelper::OnWorkspaceLoaded(clWorkspaceEvent& event)
{
    event.Skip();
    Clear();
    m_isRemoteLoaded = event.IsRemote();

    if(m_isRemoteLoaded) {
        wxString path = event.GetFileName();
        path.Replace("\\", "/");
        path = path.BeforeLast('/');
        m_workspacePath = path;
    } else {
        m_workspacePath = wxFileName(event.GetFileName()).GetPath();
    }
    m_remoteAccount = event.GetRemoteAccount();

    if(m_isRemoteLoaded && m_codeliteRemoteJSONContent.empty()) {
        // load codelite-remote.json file
        wxString codelite_remote_json_path = m_workspacePath + "/.codelite/codelite-remote.json";
#if USE_SFTP
        wxMemoryBuffer membuf;
        if(clSFTPManager::Get().AwaitReadFile(codelite_remote_json_path, m_remoteAccount, &membuf)) {
            wxString content((const char*)membuf.GetData(), wxConvUTF8, membuf.GetDataLen());
            m_codeliteRemoteJSONContent.swap(content);
            ProcessCodeLiteRemoteJSON(codelite_remote_json_path);
        }
#endif
    }
}

void CodeLiteRemoteHelper::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    Clear();
}

JSON* CodeLiteRemoteHelper::GetPluginConfig(const wxString& plugin_name) const
{
    if(m_plugins_configs.count(plugin_name) == 0) {
        return nullptr;
    }
    return m_plugins_configs.find(plugin_name)->second;
}

bool CodeLiteRemoteHelper::IsRemoteWorkspaceOpened() const { return m_isRemoteLoaded; }

void CodeLiteRemoteHelper::Clear()
{
    m_isRemoteLoaded = false;
    m_workspacePath.clear();
    m_remoteAccount.clear();
    m_codeliteRemoteJSONContent.clear();
    for(auto& vt : m_plugins_configs) {
        wxDELETE(vt.second);
    }
    m_plugins_configs.clear();
}

bool CodeLiteRemoteHelper::BuildRemoteCommand(const wxString& command, const clEnvList_t& envlist,
                                              const wxString& remote_wd, wxString* out_cmmand) const
{
    if(m_ssh_exe.empty()) {
        return false;
    }

    // build the enviroment string
    wxString envstr;
    if(!envlist.empty()) {
        for(const auto& p : envlist) {
            envstr << p.first << "=" << p.second << " ";
        }
    }
    wxString ssh_command = m_ssh_exe;
    ssh_command << " -o ServerAliveInterval=10 -o StrictHostKeyChecking=no";

    wxString cmd;
    wxString wd = remote_wd.empty() ? "$(WorkspacePath)" : remote_wd;
    cmd << ssh_command << " $(SSH_User)@$(SSH_Host) \"cd " << wd << " && " << envstr << command << "\"";
    *out_cmmand = MacroManager::Instance()->Expand(cmd, clGetManager(), wxEmptyString);
    return true;
}

#if USE_SFTP
namespace
{
void add_formatter_tool(JSONItem& tools_arr, const wxString& name, const wxString& command, const wxString& wd)
{
    auto tool = JSONItem::createObject();
    tool.addProperty("name", name);
    tool.addProperty("command", command);
    tool.addProperty("working_directory", wd);
    tools_arr.arrayAppend(tool);
}
} // namespace

void CodeLiteRemoteHelper::ProcessCodeLiteRemoteJSON(const wxString& filepath)
{
    JSON root(m_codeliteRemoteJSONContent);
    if(!root.isOk()) {
        return;
    }

    auto json = root.toElement();
    {
        // upgrade code: ensure that source code formatter
        // entries exist
        if(!json.hasNamedObject("Source Code Formatter")) {
            // missing the newly added "Source Code Formatter" section
            // add the default entry
            auto json_code_formatter = json.AddObject("Source Code Formatter");
            auto tools_arr = json_code_formatter.AddArray("tools");
            add_formatter_tool(tools_arr, "jq", "jq . -S $(CurrentFileRelPath)", "$(WorkspacePath)");
            add_formatter_tool(tools_arr, "yq", "yq . $(CurrentFileRelPath)", "$(WorkspacePath)");
            add_formatter_tool(tools_arr, "clang-format", "clang-format $(CurrentFileRelPath)", "$(WorkspacePath)");
            add_formatter_tool(tools_arr, "xmllint", "xmllint --format $(CurrentFileRelPath)", "$(WorkspacePath)");
            add_formatter_tool(tools_arr, "rustfmt", "rustfmt --edition 2021 $(CurrentFileRelPath)",
                               "$(WorkspacePath)");
            add_formatter_tool(tools_arr, "black", "black --line-length 80 $(CurrentFileRelPath)", "$(WorkspacePath)");

            // store the file
            clSFTPManager::Get().AsyncWriteFile(json.format(), filepath, m_remoteAccount);
        }
    }

    auto child = json.firstChild();
    while(child.isOk()) {
        wxString plugin_name = child.getName();
        auto p = new JSON(child.format(false));
        m_plugins_configs.insert({ plugin_name, p });
        child = json.nextChild();
    }
}
#endif
