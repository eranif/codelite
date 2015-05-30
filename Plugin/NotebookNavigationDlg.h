#ifndef NOTEBOOKNAVIGATIONDLG_H
#define NOTEBOOKNAVIGATIONDLG_H
#include "wxcrafter_plugin.h"
#include "codelite_exports.h"

class Notebook;
class WXDLLIMPEXP_SDK NotebookNavigationDlg : public NotebookNavigationDlgBase
{
    Notebook* m_book;
    int m_selection;

public:
    NotebookNavigationDlg(wxWindow* parent, Notebook* book);
    virtual ~NotebookNavigationDlg();
    void CloseDialog();

    int GetSelection() const { return m_selection; }

protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnKeyUp(wxKeyEvent& event);
};
#endif // NOTEBOOKNAVIGATIONDLG_H
