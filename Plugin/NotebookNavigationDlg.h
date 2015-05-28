#ifndef NOTEBOOKNAVIGATIONDLG_H
#define NOTEBOOKNAVIGATIONDLG_H
#include "wxcrafter_plugin.h"
#include "codelite_exports.h"

class Notebook;
class WXDLLIMPEXP_SDK NotebookNavigationDlg : public NotebookNavigationDlgBase
{
    Notebook* m_book;
public:
    NotebookNavigationDlg(wxWindow* parent, Notebook* book);
    virtual ~NotebookNavigationDlg();
    void CloseDialog();

protected:
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnKeyUp(wxKeyEvent& event);
};
#endif // NOTEBOOKNAVIGATIONDLG_H
