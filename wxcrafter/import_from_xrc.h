#ifndef IMPORTFROMXRC_H
#define IMPORTFROMXRC_H

#include "import_dlg.h"
#include "wxc_widget.h"

class wxWindow;
class NotebookPageWrapper;

class ImportFromXrc
{
public:
    ImportFromXrc(wxWindow* parent);
    ~ImportFromXrc();

    bool ImportProject(ImportDlg::ImportFileData& data) const;
    bool GetProject() const;
    static void DoProcessButtonNode(const wxXmlNode* node, wxcWidget* wrapper);
    static void ProcessBitmapProperty(const wxXmlNode* node, wxcWidget* wrapper,
                                      const wxString& property = PROP_BITMAP_PATH, const wxString& client_hint = "");
    void ProcessNamedNode(wxXmlNode* node, wxcWidget* parentwrapper,
                          const wxString& name) const; // Used for auitoolbar dropdown nodes

protected:
    bool ParseFile(wxXmlDocument& doc, wxcWidget::List_t& toplevels) const;
    wxcWidget* ParseNode(wxXmlNode* node, wxcWidget* parentwrapper, bool& alreadyParented) const;
    void GetSizeritemContents(const wxXmlNode* node, wxcWidget* wrapper) const;
    void GetBookitemContents(const wxXmlNode* node, NotebookPageWrapper* wrapper, int& depth) const;
    void ProcessButtonNode(const wxXmlNode* node, wxcWidget* wrapper) const;
    wxWindow* m_Parent;
};

#endif // IMPORTFROMXRC_H
