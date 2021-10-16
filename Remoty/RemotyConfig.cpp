#include "RemotyConfig.hpp"
#include "cl_config.h"

namespace
{
size_t MAX_ITEMS = 15;
}

RemotyConfig::RemotyConfig() {}

RemotyConfig::~RemotyConfig() {}

wxArrayString RemotyConfig::GetRecentWorkspaces() const
{
    return clConfig::Get().Read("remoty/recent_workspaces", wxArrayString{});
}

void RemotyConfig::UpdateRecentWorkspaces(const wxString& workspace)
{
    if(workspace.empty()) {
        return;
    }

    auto curitems = GetRecentWorkspaces();
    if(curitems.Index(workspace) != wxNOT_FOUND) {
        curitems.Remove(workspace);
    }
    curitems.Insert(workspace, 0);
    if(curitems.size() > MAX_ITEMS) {
        // shrink the list
        curitems.resize(MAX_ITEMS);
    }
    clConfig::Get().Write("remoty/recent_workspaces", curitems);
}
