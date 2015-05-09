#ifndef CODEBLOCKSIMPORTER_H
#define CODEBLOCKSIMPORTER_H

#include <wx/filename.h>
#include <wx/string.h>
#include "GenericImporter.h"

class CodeBlocksImporter : public GenericImporter
{
public:
    virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler);
    virtual bool isSupportedWorkspace();
    virtual GenericWorkspacePtr PerformImport();

private:
    void GenerateFromWorkspace(GenericWorkspacePtr genericWorkspace);
    void GenerateFromProject(GenericWorkspacePtr genericWorkspace, GenericProjectDataType& genericProjectData);

    wxFileName wsInfo;
    wxString extension;
};

#endif // CODEBLOCKSIMPORTER_H
