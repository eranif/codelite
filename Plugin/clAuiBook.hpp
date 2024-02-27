#ifndef CLAUIBOOK_HPP
#define CLAUIBOOK_HPP

#include "Notebook.h"
#include "codelite_exports.h"

#include <wx/aui/auibook.h>

class WXDLLIMPEXP_SDK clAuiBook : public wxAuiNotebook
{
public:
    clAuiBook(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize, long style = 0);
    ~clAuiBook() override;
    size_t GetAllTabs(clTabInfo::Vec_t& tabs);
    clTabHistory::Ptr_t GetHistory() const { return m_history; }
    void MoveActivePage(int newIndex);
    int SetSelection(size_t newPage) override;
    int ChangeSelection(size_t n) override;
    wxBorder GetDefaultBorder() const override;

protected:
    void OnPageClosed(wxAuiNotebookEvent& event);
    void OnPageClosing(wxAuiNotebookEvent& event);
    void OnPageChanged(wxAuiNotebookEvent& event);
    void OnPageChanging(wxAuiNotebookEvent& event);
    void OnPageRightDown(wxAuiNotebookEvent& event);

private:
    clTabHistory::Ptr_t m_history;
};

#endif // CLAUIBOOK_HPP
