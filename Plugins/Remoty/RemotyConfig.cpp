#include "RemotyConfig.hpp"

#include "cl_config.h"
#include "file_logger.h"

#include <algorithm>

namespace
{
size_t MAX_ITEMS = 15;
const wxString REMOTY_RECENT_WORKSPACES = "remoty/recent_remote_workspaces";
const wxString REMOTY_OPEN_WORKSPACE_TYPE = "remoty/open_workspace_type";
const wxString REMOTY_LAST_WORKSPACE_ACCOUNT = "remoty/last_workspace/account";
const wxString REMOTY_LAST_WORKSPACE_PATH = "remoty/last_workspace/path";
} // namespace

std::vector<RemoteWorkspaceInfo> RemotyConfig::GetRecentWorkspaces() const
{
    std::vector<RemoteWorkspaceInfo> res;
    clConfig::Get().Read(REMOTY_RECENT_WORKSPACES, [&res](const JSONItem& item) {
        size_t count = item.arraySize();
        if (count) {
            res.reserve(count);
        }
        for (size_t i = 0; i < count; ++i) {
            JSONItem n = item.arrayItem(i);
            RemoteWorkspaceInfo d;
            d.account = n["account"].toString();
            d.path = n["path"].toString();
            res.emplace_back(std::move(d));
        }
    });
    return res;
}

void RemotyConfig::UpdateRecentWorkspaces(const RemoteWorkspaceInfo& workspaceInfo)
{
    if (workspaceInfo.path.empty() || workspaceInfo.account.empty()) {
        return;
    }

    auto curitems = GetRecentWorkspaces();
    auto where = find_if(curitems.begin(), curitems.end(), [&workspaceInfo](const RemoteWorkspaceInfo& d) {
        return d.path == workspaceInfo.path && d.account == workspaceInfo.account;
    });
    if (where != curitems.end()) {
        curitems.erase(where);
    }

    curitems.insert(curitems.begin(), std::move(workspaceInfo));
    if (curitems.size() > MAX_ITEMS) {
        // shrink the list
        curitems.resize(MAX_ITEMS);
    }

    // serialise the items
    clConfig::Get().Write(REMOTY_RECENT_WORKSPACES, [&curitems]() -> JSONItem {
        clDEBUG() << "Writing remote workspaces to disk" << endl;
        JSONItem arr = JSONItem::createArray();
        for (const auto& wi : curitems) {
            auto d = JSONItem::createObject();
            d.addProperty("account", wi.account);
            d.addProperty("path", wi.path);
            arr.arrayAppend(std::move(d));
        }
        clDEBUG() << "Array content:" << arr.FormatRawString() << endl;
        return arr;
    });
    clConfig::Get().Save();
}

bool RemotyConfig::IsOpenWorkspaceTypeLocal() const { return clConfig::Get().Read(REMOTY_OPEN_WORKSPACE_TYPE, true); }

void RemotyConfig::SetOpenWorkspaceTypeLocal(bool local) { clConfig::Get().Write(REMOTY_OPEN_WORKSPACE_TYPE, local); }

RemoteWorkspaceInfo RemotyConfig::GetLastWorkspace() const
{
    RemoteWorkspaceInfo info;
    info.account = clConfig::Get().Read(REMOTY_LAST_WORKSPACE_ACCOUNT, wxString());
    info.path = clConfig::Get().Read(REMOTY_LAST_WORKSPACE_PATH, wxString());
    return info;
}

void RemotyConfig::SetLastWorkspace(const RemoteWorkspaceInfo& workspaceInfo)
{
    clConfig::Get().Write(REMOTY_LAST_WORKSPACE_ACCOUNT, workspaceInfo.account);
    clConfig::Get().Write(REMOTY_LAST_WORKSPACE_PATH, workspaceInfo.path);
}
