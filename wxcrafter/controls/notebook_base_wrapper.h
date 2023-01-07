#ifndef NOTEBOOKBASEWRAPPER_H
#define NOTEBOOKBASEWRAPPER_H

#include "wxc_widget.h" // Base class: WrapperBase
class NotebookPageWrapper;
// Base wrapper for all the Notebook classes
// Note that this class is pure virtual and can not be instantiated
class NotebookBaseWrapper : public wxcWidget
{
public:
    NotebookBaseWrapper(int id);
    virtual ~NotebookBaseWrapper();

protected:
    void DoSetSelection(NotebookPageWrapper* page, wxcWidget* pageToSelect);
    NotebookPageWrapper* DoGetSelection(NotebookPageWrapper* page) const;
    bool DoGetPageIndex(const NotebookPageWrapper* page, const NotebookPageWrapper* pageToFind, int& count) const;
    NotebookPageWrapper* GetLastPage() const;
    NotebookPageWrapper* DoGetChildPageAtDepth(NotebookPageWrapper* page, size_t targetlevel,
                                               size_t currentlevel) const;

public:
    virtual wxString CppCtorCode() const;
    virtual void ToXRC(wxString& text, XRC_TYPE type) const;
    virtual wxString GetXRCPageClass() const = 0;
    void SetSelection(wxcWidget* page);
    NotebookPageWrapper* GetSelection() const;
    wxcWidget* GetChildPageAtDepth(size_t level);

    int GetPageIndex(const NotebookPageWrapper* page) const;
};

#endif // NOTEBOOKWRAPPER_H
