#ifndef VISUALCPPIMPORTER_H
#define VISUALCPPIMPORTER_H

#include <wx/filename.h>
#include <wx/wfstream.h>
#include <wx/txtstrm.h>
#include <wx/string.h>
#include <memory>
#include "GenericImporter.h"

class VisualCppImporter : public GenericImporter
{
public:
    VisualCppImporter();
    ~VisualCppImporter();

    virtual bool OpenWordspace(const wxString& filename, const wxString& defaultCompiler);
    virtual bool isSupportedWorkspace();
    virtual GenericWorkspacePtr PerformImport();

private:
    void GenerateFromVC6(GenericWorkspacePtr genericWorkspace);
    void GenerateFromVC7(GenericWorkspacePtr genericWorkspace);
    void GenerateFromVC11(GenericWorkspacePtr genericWorkspace);

    wxFileName wsInfo;
    long version;
    bool IsGccCompile;
    std::shared_ptr<wxFileInputStream> fis;
    std::shared_ptr<wxTextInputStream> tis;
};

#endif // VISUALCPPIMPORTER_H
