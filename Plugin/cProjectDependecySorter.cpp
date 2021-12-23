#include "cProjectDependecySorter.h"

#include "cl_exception.h"
#include "macros.h"

#include <algorithm>
#include <workspace.h>

clProjectDependecySorter::clProjectDependecySorter() {}

clProjectDependecySorter::~clProjectDependecySorter() {}

void clProjectDependecySorter::Visit(clProjectDependecySorter::Node* node, wxArrayString& buildOrder)
{
    if(node->marker == kPerm)
        return;
    if(node->marker == kTemp)
        throw clException("Dependency loop found for node: " + node->name);

    node->marker = kTemp;
    std::for_each(node->adjacents.begin(), node->adjacents.end(), [&](Node* adj) { Visit(adj, buildOrder); });
    node->marker = kPerm;
    buildOrder.Insert(node->name, 0); // add as the first entry
}

void clProjectDependecySorter::GetProjectBuildOrder(const wxString& projectName, const wxString& configName,
                                                    wxArrayString& buildOrder)
{
    Graph_t G;
    wxArrayString projects;
    clCxxWorkspaceST::Get()->GetProjectList(projects);
    for(size_t i = 0; i < projects.size(); ++i) {
        ProjectPtr p = clCxxWorkspaceST::Get()->GetProject(projects.Item(i));
        if(!p) {
            throw clException("Could not find project: " + projects.Item(i));
        }
        Node* projectNode = GetNodeCreateIfNeeded(G, p->GetName());
        wxArrayString deps = p->GetDependencies(configName);
        std::for_each(deps.begin(), deps.end(), [&](const wxString& dep) {
            Node* adj = GetNodeCreateIfNeeded(G, dep);
            adj->adjacents.push_back(projectNode);
        });
    }

    std::for_each(G.begin(), G.end(), [&](Graph_t::value_type& vt) {
        Node* n = &(vt.second);
        if(n->marker == clProjectDependecySorter::kNone) {
            Visit(n, buildOrder);
        }
    });
}

clProjectDependecySorter::Node* clProjectDependecySorter::GetNodeCreateIfNeeded(Graph_t& G, const wxString& name)
{
    std::string key = name.mb_str(wxConvUTF8).data();
    if(G.count(key) == 0) {
        clProjectDependecySorter::Node n;
        n.name = key;
        G[key] = n;
    }
    return &G[key];
}
