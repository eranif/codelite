#include "my_tree_ctrl.h"

MyTreeCtrl::MyTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : wxDataViewTreeCtrl(parent, id, pos, size, style)
    , m_next(0)
{
}

MyTreeCtrl::~MyTreeCtrl() {}

wxDataViewItem MyTreeCtrl::AddRoot(const wxString& name, int imgId, int expandImgId)
{
    return AppendContainer(wxDataViewItem(0), name, imgId, expandImgId);
}

void MyTreeCtrl::SelectItem(const wxDataViewItem& item, bool select)
{
    if(select)
        Select(item);
    else
        Unselect(item);
}

wxDataViewItem MyTreeCtrl::GetFirstChild(const wxDataViewItem& item)
{
    m_children.clear();
    GetStore()->GetChildren(item, m_children);
    if(m_children.IsEmpty())
        return wxDataViewItem();
    else {
        m_next = 1;
        return m_children.Item(0);
    }
}

wxDataViewItem MyTreeCtrl::GetNextChild(const wxDataViewItem& item)
{
    if(m_children.IsEmpty()) return wxDataViewItem();

    if(m_next >= m_children.GetCount()) return wxDataViewItem();

    wxDataViewItem chld = m_children.Item(m_next);
    m_next++;
    return chld;
}

bool MyTreeCtrl::ItemHasChildren(const wxDataViewItem& item) const
{
    return IsContainer(item) && GetStore()->GetChildCount(item);
}
