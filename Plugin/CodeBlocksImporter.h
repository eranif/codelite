#ifndef CODEBLOCKSIMPORTER_H
#define CODEBLOCKSIMPORTER_H

#include <wx/filename.h>
#include <wx/string.h>
#include "GenericImporter.h"

class CodeBlocksImporter : public GenericImporter
{
public:    
    CodeBlocksImporter();
    ~CodeBlocksImporter();
	
	virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler);
	virtual bool isSupportedWorkspace();
    virtual GenericWorkspacePtr PerformImport();
	
private:
    void GenerateFromProject(GenericWorkspacePtr genericWorkspace, const wxString& fullpath);
    void GenerateFromWorkspace(GenericWorkspacePtr genericWorkspace);
	
    wxFileName wsInfo;
    bool IsGccCompile;
	wxString extension;
};

#endif // CODEBLOCKSIMPORTER_H
