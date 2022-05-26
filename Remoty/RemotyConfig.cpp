#include "RemotyConfig.hpp"

#include "cl_config.h"

#include <algorithm>

namespace
{
size_t MAX_ITEMS = 15;
const wxString REMOTY_RECENT_WORKSPACES = "remoty/recent_remote_workspaces";
const wxString REMOTY_OPEN_WORKSPACE_TYPE = "remoty/open_workspace_type";
} // namespace

RemotyConfig::RemotyConfig() {}

RemotyConfig::~RemotyConfig() {}

std::vector<RemoteWorkspaceInfo> RemotyConfig::GetRecentWorkspaces() const
{
    std::vector<RemoteWorkspaceInfo> res;
    clConfig::Get().Read(REMOTY_RECENT_WORKSPACES, [&res](const JSONItem& item) {
        size_t count = item.arraySize();
        if(count) {
            res.reserve(count);
        }
        for(size_t i = 0; i < count; ++i) {
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
    if(workspaceInfo.path.empty() || workspaceInfo.account.empty()) {
        return;
    }

    auto curitems = GetRecentWorkspaces();
    auto where = find_if(curitems.begin(), curitems.end(), [&workspaceInfo](const RemoteWorkspaceInfo& d) {
        return d.path == workspaceInfo.path && d.account == workspaceInfo.account;
    });
    if(where != curitems.end()) {
        curitems.erase(where);
    }

    curitems.insert(curitems.begin(), std::move(workspaceInfo));
    if(curitems.size() > MAX_ITEMS) {
        // shrink the list
        curitems.resize(MAX_ITEMS);
    }

    // serialise the items
    clConfig::Get().Write(REMOTY_RECENT_WORKSPACES, [&curitems]() -> JSONItem {
        JSONItem arr = JSONItem::createArray();
        for(const auto& wi : curitems) {
            auto d = arr.AddObject(wxEmptyString);
            d.addProperty("account", wi.account);
            d.addProperty("path", wi.path);
        }
        return arr;
    });
}

bool RemotyConfig::IsOpenWorkspaceTypeLocal() const { return clConfig::Get().Read(REMOTY_OPEN_WORKSPACE_TYPE, true); }

void RemotyConfig::SetOpenWorkspaceTypeLocal(bool local) { clConfig::Get().Write(REMOTY_OPEN_WORKSPACE_TYPE, local); }
