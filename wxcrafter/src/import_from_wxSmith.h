#ifndef IMPORTFROMSMITH_H
#define IMPORTFROMSMITH_H

#include "import_dlg.h"
#include "wxc_widget.h"
#include "wxgui_defs.h"
#include <map>

class wxWindow;
class NotebookPageWrapper;

class ImportFromwxSmith
{
    static std::map<wxString, wxString> sm_eventMap;
    wxWindow* m_Parent;
    bool m_loadWhenDone;

public:
    ImportFromwxSmith(wxWindow* parent);
    ~ImportFromwxSmith();

    bool ImportProject(ImportDlg::ImportFileData& data, const wxString& sourceFile = "") const;
    bool GetProject() const;

    bool IsLoadWhenDone() const { return m_loadWhenDone; }

protected:
    bool ParseFile(wxXmlDocument& doc, wxcWidget::List_t& toplevels) const;
    wxcWidget* ParseNode(wxXmlNode* node, wxcWidget* parentwrapper, bool& alreadyParented) const;
    void GetSizeritemContents(const wxXmlNode* node, wxcWidget* wrapper) const;
    void GetGridBagSizerData(const wxXmlNode* node, wxcWidget* wrapper) const;
    void GetBookitemContents(const wxXmlNode* node, NotebookPageWrapper* wrapper, int& depth) const;
};

#endif // IMPORTFROMSMITH_H
