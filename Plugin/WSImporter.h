#ifndef WSIMPORTER_H
#define WSIMPORTER_H

#include <wx/string.h>
#include <vector>
#include <set>
#include <memory>
#include "GenericImporter.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK WSImporter
{
public:
    WSImporter();
    ~WSImporter();

    void Load(const wxString& filename, const wxString& defaultCompiler);
    bool Import(wxString& errMsg);

protected:
    void AddImporter(std::shared_ptr<GenericImporter> importer);

private:
    bool ContainsEnvVar(std::vector<wxString> elems);
    std::set<wxString> GetListEnvVarName(std::vector<wxString> elems);
    wxString GetVPath(const wxString& filename, const wxString& virtualPath, const bool& createDefaultVDir);

    wxString filename, defaultCompiler;
    std::vector<std::shared_ptr<GenericImporter> > importers;
};

#endif // WSIMPORTER_H
