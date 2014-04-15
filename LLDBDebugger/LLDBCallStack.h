#ifndef LLDBCALLSTACK_H
#define LLDBCALLSTACK_H

#include "UI.h"
#include "LLDBProtocol/LLDBBacktrace.h"
#include "LLDBProtocol/LLDBEvent.h"

class LLDBCallStackPane;
class LLDBConnector;

class CallstackModel : public wxDataViewListStore
{
    LLDBCallStackPane* m_ctrl;
    wxDataViewListCtrl* m_view;
public:
    CallstackModel(LLDBCallStackPane* ctrl, wxDataViewListCtrl* view) : m_ctrl(ctrl), m_view(view) {}
    virtual ~CallstackModel() {}
    
    bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const ;
};

class LLDBCallStackPane : public LLDBCallStackBase
{
    LLDBConnector* m_connector;
    int m_selectedFrame;
    wxObjectDataPtr<CallstackModel> m_model;
    
protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
    void OnBacktrace(LLDBEvent &event);
    void OnRunning(LLDBEvent &event);

public:
    LLDBCallStackPane(wxWindow* parent, LLDBConnector* connector);
    virtual ~LLDBCallStackPane();
    
    void SetSelectedFrame(int selectedFrame) {
        this->m_selectedFrame = selectedFrame;
    }
    int GetSelectedFrame() const {
        return m_selectedFrame;
    }
};

#endif // LLDBCALLSTACK_H
