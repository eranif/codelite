#ifndef NOTEBOOKPAGEWRAPPER_H
#define NOTEBOOKPAGEWRAPPER_H

#include "wxc_widget.h"
class NotebookBaseWrapper;
class NotebookPageWrapper : public wxcWidget
{
    bool m_selected;

protected:
    void DoTreebookXRC(wxString& text, XRC_TYPE type, int depth) const;
    virtual void LoadPropertiesFromXRC(const wxXmlNode* node);
    virtual void LoadPropertiesFromwxSmith(const wxXmlNode* node);
    wxString DoTreebookCppCtorCode() const;

public:
    NotebookPageWrapper();
    virtual ~NotebookPageWrapper();
    virtual void SetParent(wxcWidget* parent);

    bool IsTreebookPage() const;
    bool IsChoicebookPage() const;
    NotebookBaseWrapper* GetNotebook() const;

    int GetPageIndex() const;

    wxcWidget* Clone() const;
    wxString CppCtorCode() const;
    void GetIncludeFile(wxArrayString& headers) const;
    wxString GetWxClassName() const;
    void ToXRC(wxString& text, XRC_TYPE type) const;

    // This class is a container
    virtual bool IsContainer() const { return true; }

    void SetSelected(bool selected) { this->m_selected = selected; }
    bool IsSelected() const { return m_selected; }

    wxSize GetImageSize() const;
};

#endif // NOTEBOOKPAGEWRAPPER_H
