#ifndef CPROJECTDEPENDECYSORTER_H
#define CPROJECTDEPENDECYSORTER_H

#include <vector>
#include "codelite_exports.h"
#include <wx/arrstr.h>
#include <unordered_map>
#include "cl_exception.h"

class WXDLLIMPEXP_SDK clProjectDependecySorter
{
    enum eVisit { kNone, kTemp, kPerm };
    struct Node
    {
        eVisit marker;
        wxString name;
        std::vector<Node*> adjacents;
        Node()
            : marker(kNone)
        {
        }
    };
    typedef std::unordered_map<wxString, clProjectDependecySorter::Node> Graph_t;

protected:
    void Visit(clProjectDependecySorter::Node* node, wxArrayString& buildOrder) throw(clException);
    clProjectDependecySorter::Node* GetNodeCreateIfNeeded(Graph_t& G, const wxString& name);

public:
    clProjectDependecySorter();
    virtual ~clProjectDependecySorter();

    /**
     * @brief return the build order for a given project taking its dependencies into consideration
     * @param projectName
     * @param buildOrder [output]
     * @return the build order. Throws clException in case of an error
     */
    void GetProjectBuildOrder(const wxString& projectName, const wxString& configName,
                              wxArrayString& buildOrder) throw(clException);
};

#endif // CPROJECTDEPENDECYSORTER_H
