#ifndef BORLANDCPPBUILDERIMPORTER_H
#define BORLANDCPPBUILDERIMPORTER_H

#include <wx/filename.h>
#include <wx/string.h>
#include <memory>
#include "GenericImporter.h"

class BorlandCppBuilderImporter : public GenericImporter
{
public:
    BorlandCppBuilderImporter();
    ~BorlandCppBuilderImporter();
	
	virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler);
	virtual bool isSupportedWorkspace();
    virtual GenericWorkspacePtr PerformImport();
	
private:
	wxFileName wsInfo;
	bool IsGccCompile;
};

#endif // BORLANDCPPBUILDERIMPORTER_H
