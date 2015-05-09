#ifndef VISUALCPPIMPORTER_H
#define VISUALCPPIMPORTER_H

#include <wx/filename.h>
#include <wx/string.h>
#include "GenericImporter.h"

class VisualCppImporter : public GenericImporter
{
public:
    virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler);
    virtual bool isSupportedWorkspace();
    virtual GenericWorkspacePtr PerformImport();

private:
    void GenerateFromVC6(GenericWorkspacePtr genericWorkspace);
    void GenerateFromProjectVC6(GenericWorkspacePtr genericWorkspace, GenericProjectDataType& genericProjectData);
    void GenerateFromVC7_11(GenericWorkspacePtr genericWorkspace);
    void GenerateFromProjectVC7(GenericWorkspacePtr genericWorkspace, GenericProjectDataType& genericProjectData);
    void GenerateFromProjectVC11(GenericWorkspacePtr genericWorkspace, GenericProjectDataType& genericProjectData);
    wxString ExtractProjectCfgName(const wxString& parentCondition, const wxString& elemCondition);
    wxString ReplaceDefaultEnvVars(const wxString& str);

    wxFileName wsInfo;
    wxString extension;
    long version;
};

#endif // VISUALCPPIMPORTER_H
