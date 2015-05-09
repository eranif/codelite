#ifndef DEVCPPIMPORTER_H
#define DEVCPPIMPORTER_H

#include <wx/filename.h>
#include <wx/string.h>
#include "GenericImporter.h"

class DevCppImporter : public GenericImporter
{
public:
    virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler);
    virtual bool isSupportedWorkspace();
    virtual GenericWorkspacePtr PerformImport();

private:
    wxFileName wsInfo;
};

#endif // DEVCPPIMPORTER_H
