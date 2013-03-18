#ifndef LOCALSVIEW_H
#define LOCALSVIEW_H

#include "debugger_ui.h"
#include <map>
#include <set>

class LocalsView : public LocalsViewBase
{
    std::map<wxString, wxDataViewItem> m_gdbIdToDVItem;
    //std::map<wxString, wxDataViewItem> m_waitingForEvaluate;
    std::set<wxDataViewItem>           m_locals;
    std::map<wxString, wxDataViewItem> m_waitingForCreateObj;

public:
    LocalsView(wxWindow* parent);
    virtual ~LocalsView();
    void Clear();
    
    void OnLocals(wxCommandEvent &e);
    void OnRefreshView(wxCommandEvent &e);
    void OnCreateVariableObject(wxCommandEvent &e);
    void OnListChildren(wxCommandEvent &e);
    void OnEvaluate(wxCommandEvent &e);

protected:
    virtual void OnItemExpanding(wxDataViewEvent& event);
};
#endif // LOCALSVIEW_H
