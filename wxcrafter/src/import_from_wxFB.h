#ifndef IMPORTFROMFB_H
#define IMPORTFROMFB_H

#include "import_dlg.h"
#include "wxc_widget.h"
#include "wxgui_defs.h"
#include <map>

class wxWindow;
class NotebookPageWrapper;

class ImportFromwxFB
{
    static std::map<wxString, wxString> sm_eventMap;
    wxWindow* m_Parent;
    bool m_loadWhenDone;

public:
    ImportFromwxFB(wxWindow* parent);
    ~ImportFromwxFB();

    bool ImportProject(ImportDlg::ImportFileData& data, const wxString& sourceFile = "") const;
    static wxString ConvertFBOptionsString(const wxString& content, const wxString& separator = ";");
    static void ProcessBitmapProperty(const wxString& value, wxcWidget* wrapper,
                                      const wxString& property = PROP_BITMAP_PATH, const wxString& client_hint = "");
    bool GetProject() const;
    static wxString GetEventtypeFromHandlerstub(const wxString& stub);

    bool IsLoadWhenDone() const { return m_loadWhenDone; }

protected:
    bool ParseFile(wxXmlDocument& doc, wxcWidget::List_t& toplevels) const;
    wxcWidget* ParseNode(wxXmlNode* node, wxcWidget* parentwrapper, bool& alreadyParented) const;
    void GetSizeritemContents(const wxXmlNode* node, wxcWidget* wrapper) const;
    void GetGridBagSizerItem(const wxXmlNode* node, wxcWidget* wrapper) const;
    void GetBookitemContents(const wxXmlNode* node, NotebookPageWrapper* wrapper, int& depth) const;
};

#endif // IMPORTFROMFB_H
