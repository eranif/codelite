#ifndef DEVCPPIMPORTER_H
#define DEVCPPIMPORTER_H

#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/string.h>
#include <memory>
#include "GenericImporter.h"

class DevCppImporter : public GenericImporter
{
public:
    virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler);
    virtual bool isSupportedWorkspace();
    virtual GenericWorkspacePtr PerformImport();

private:
    wxFileName wsInfo;
    bool IsGccCompile;
    std::shared_ptr<wxFileInputStream> fis;
    std::shared_ptr<wxTextInputStream> tis;
};

#endif // DEVCPPIMPORTER_H
