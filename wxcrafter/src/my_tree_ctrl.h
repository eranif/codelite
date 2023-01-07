#ifndef MYTREECTRL_H
#define MYTREECTRL_H

#include "wx/dataview.h" // Base class: wxDataViewTreeCtrl

// Proxy class for wxTreeCtrl based on wxDataViewTreeCtrl
class MyTreeCtrl : public wxDataViewTreeCtrl
{
    wxDataViewItemArray m_children;
    size_t m_next;

public:
    MyTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize, long style = wxDV_NO_HEADER | wxDV_SINGLE | wxDV_ROW_LINES);
    virtual ~MyTreeCtrl();

    wxDataViewItem AddRoot(const wxString& name, int imgId = -1, int expandImgId = -1);
    void SelectItem(const wxDataViewItem& item, bool select);
    wxDataViewItem GetFirstChild(const wxDataViewItem& item);
    wxDataViewItem GetNextChild(const wxDataViewItem& item);
    bool ItemHasChildren(const wxDataViewItem& item) const;
};

#endif // MYTREECTRL_H
