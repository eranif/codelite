#ifndef CLTREEKEYBOARDINPUT_H
#define CLTREEKEYBOARDINPUT_H

#include "codelite_exports.h"
#include "wx/event.h"
#include <wx/sharedptr.h>
#include <wx/treebase.h>
#include <list>
#include <vector>

class wxTextCtrl;
class wxTreeCtrl;
class WXDLLIMPEXP_SDK clTreeKeyboardInput : public wxEvtHandler
{

public:
    typedef wxSharedPtr<clTreeKeyboardInput> Ptr_t;

    enum eSearchFlags {
        kNone = 0,             // 0
        kRecursive = (1 << 0), // 1
    };

protected:
    wxTreeCtrl* m_tree;
    wxTextCtrl* m_text;
    eSearchFlags m_flags;
    std::list<wxTreeItemId> m_items;
    
protected:
    void OnKeyDown(wxKeyEvent& event);
    void OnTextKeyDown(wxKeyEvent& event);
    void OnTextUpdated(wxCommandEvent& event);
    void OnTextEnter(wxCommandEvent& event);
    void OnTreeFocus(wxFocusEvent& event);

    void SelecteItem(const wxTreeItemId& item);
    bool CheckItemForMatch(const wxTreeItemId& item);
    void SetTextFocus();
    void GetChildren(const wxTreeItemId& from = wxTreeItemId(), const wxTreeItemId& until = wxTreeItemId());
    void DoGetChildren(const wxTreeItemId& parent);
    void Clear();
    void DoShowTextBox();
    
public:
    clTreeKeyboardInput(wxTreeCtrl* tree, eSearchFlags flags = kNone);
    virtual ~clTreeKeyboardInput();
};

#endif // CLTREEKEYBOARDINPUT_H
