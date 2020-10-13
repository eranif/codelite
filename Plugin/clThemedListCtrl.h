#ifndef CLTHEMEDLISTCTRL_H
#define CLTHEMEDLISTCTRL_H

#include "clDataViewListCtrl.h"
#include "clTreeKeyboardInput.h"
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clThemedListCtrlBase : public clDataViewListCtrl
{
protected:
    clTreeKeyboardInput::Ptr_t m_keyboard;

protected:
    void OnThemeChanged(wxCommandEvent& event);
    void ApplyTheme();
    void Initialise(bool disableSorting);

public:
    clThemedListCtrlBase(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clThemedListCtrlBase();
};

class WXDLLIMPEXP_SDK clThemedListCtrl : public clThemedListCtrlBase
{
public:
    clThemedListCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clThemedListCtrl();
};

class WXDLLIMPEXP_SDK clThemedOrderedListCtrl : public clThemedListCtrlBase
{
    size_t m_sortedColumn = 0;

public:
    clThemedOrderedListCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~clThemedOrderedListCtrl();

    void SetSortedColumn(size_t sortedColumn) { this->m_sortedColumn = sortedColumn; }
    size_t GetSortedColumn() const { return m_sortedColumn; }
};

#endif // CLTHEMEDLISTCTRL_H
