#ifndef IMPORTERFROMFB_H
#define IMPORTERFROMFB_H

#include "import_dlg.h"
#include "wxc_widget.h"
#include "wxgui_defs.h"

class wxWindow;
class NotebookPageWrapper;

namespace ImportFromwxFB
{

class Importer
{
public:
    Importer(wxWindow* parent);
    ~Importer() = default;

    bool ImportProject(ImportDlg::ImportFileData& data, const wxString& sourceFile, bool showAddToProject) const;

protected:
    bool ParseFile(wxXmlDocument& doc, wxcWidget::List_t& toplevels) const;
    wxcWidget* ParseNode(wxXmlNode* node, wxcWidget* parentwrapper, bool& alreadyParented) const;
    void GetSizeritemContents(const wxXmlNode* node, wxcWidget* wrapper) const;
    void GetGridBagSizerItem(const wxXmlNode* node, wxcWidget* wrapper) const;
    void GetBookitemContents(const wxXmlNode* node, NotebookPageWrapper* wrapper, int& depth) const;

private:
    wxWindow* m_Parent = nullptr;
};

} // namespace ImportFromwxFB

#endif // IMPORTERFROMFB_H
