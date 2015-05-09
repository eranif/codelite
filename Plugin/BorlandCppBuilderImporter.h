#ifndef BORLANDCPPBUILDERIMPORTER_H
#define BORLANDCPPBUILDERIMPORTER_H

#include <wx/filename.h>
#include <wx/string.h>
#include "GenericImporter.h"

class BorlandCppBuilderImporter : public GenericImporter
{
public:
    virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler);
    virtual bool isSupportedWorkspace();
    virtual GenericWorkspacePtr PerformImport();

private:
    wxFileName wsInfo;
};

#endif // BORLANDCPPBUILDERIMPORTER_H
