#ifndef IMPORTERFROMSMITH_H
#define IMPORTERFROMSMITH_H

#include "UI/import_dlg.h"
#include "wxc_widget.h"

#include <map>

class wxWindow;
class NotebookPageWrapper;

namespace ImportFromwxSmith
{

class Importer
{
    static std::map<wxString, wxString> sm_eventMap;
    wxWindow* m_Parent = nullptr;

public:
    Importer(wxWindow* parent);
    ~Importer() = default;

    bool ImportProject(ImportDlg::ImportFileData& data, const wxString& sourceFile, bool showAddToProject) const;

protected:
    bool ParseFile(wxXmlDocument& doc, wxcWidget::List_t& toplevels) const;
    wxcWidget* ParseNode(wxXmlNode* node, wxcWidget* parentwrapper, bool& alreadyParented) const;
    void GetSizeritemContents(const wxXmlNode* node, wxcWidget* wrapper) const;
    void GetGridBagSizerData(const wxXmlNode* node, wxcWidget* wrapper) const;
    void GetBookitemContents(const wxXmlNode* node, NotebookPageWrapper* wrapper, int& depth) const;
};
} // namespace ImportFromwxSmith

#endif // IMPORTERFROMSMITH_H
