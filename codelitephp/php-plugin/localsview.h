#ifndef LOCALSVIEW_H
#define LOCALSVIEW_H

#include "php_ui.h"
#include <macros.h>
#include "xdebugevent.h"
#include <map>

class LocalsView : public LocalsViewBase
{
    /// Set if items that are expanded in the current locals view
    wxStringSet_t       m_localsExpandedItemsFullname;
    wxDataViewItemArray m_localsExpandedItems;
    std::map<wxString, wxDataViewItem> m_waitingExpand;
    
    
public:
    LocalsView(wxWindow* parent);
    virtual ~LocalsView();
    
protected:
    virtual void OnLocalExpanding(wxDataViewEvent& event);
    virtual void OnLocalCollapsed(wxDataViewEvent& event);
    virtual void OnLocalExpanded(wxDataViewEvent& event);
    
    wxString DoGetItemClientData(const wxDataViewItem& item) const;
    
    void OnLocalsUpdated(XDebugEvent& e);
    void OnXDebugSessionEnded(XDebugEvent &e);
    void OnXDebugSessionStarted(XDebugEvent &e);
    void OnProperytGet(XDebugEvent &e);
    void AppendVariablesToTree(const wxDataViewItem& parent, const XVariable::List_t& children);
};
#endif // LOCALSVIEW_H
