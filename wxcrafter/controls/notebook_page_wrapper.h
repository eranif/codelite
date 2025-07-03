#ifndef NOTEBOOKPAGEWRAPPER_H
#define NOTEBOOKPAGEWRAPPER_H

#include "wxc_widget.h"
class NotebookBaseWrapper;
class NotebookPageWrapper : public wxcWidget
{
    bool m_selected;

protected:
    void LoadPropertiesFromXRC(const wxXmlNode* node) override;
    void LoadPropertiesFromwxSmith(const wxXmlNode* node) override;

    void DoTreebookXRC(wxString& text, XRC_TYPE type, int depth) const;
    wxString DoTreebookCppCtorCode() const;

public:
    NotebookPageWrapper();
    ~NotebookPageWrapper() override = default;
    void SetParent(wxcWidget* parent) override;

    bool IsTreebookPage() const;
    bool IsChoicebookPage() const;
    NotebookBaseWrapper* GetNotebook() const;

    int GetPageIndex() const;

    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
    void GetIncludeFile(wxArrayString& headers) const override;
    wxString GetWxClassName() const override;
    void ToXRC(wxString& text, XRC_TYPE type) const override;

    // This class is a container
    bool IsContainer() const override { return true; }

    void SetSelected(bool selected) { this->m_selected = selected; }
    bool IsSelected() const { return m_selected; }

    wxSize GetImageSize() const;
};

#endif // NOTEBOOKPAGEWRAPPER_H
