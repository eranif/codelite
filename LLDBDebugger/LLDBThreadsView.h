#ifndef LLDBTHREADSVIEW_H
#define LLDBTHREADSVIEW_H

#include "UI.h"
#include "LLDBProtocol/LLDBEvent.h"
#include "LLDBProtocol/LLDBThread.h"

class LLDBPlugin;

//-------------------------------------------------------------
//-------------------------------------------------------------

class LLDBThreadViewClientData : public wxClientData
{
    LLDBThread m_thread;
public:
    LLDBThreadViewClientData(const LLDBThread& thread) {
        m_thread = thread;
    }
    const LLDBThread& GetThread() const {
        return m_thread;
    }
    
};

//-------------------------------------------------------------
//-------------------------------------------------------------

class ThreadsModel : public wxDataViewListStore
{
    wxDataViewListCtrl* m_view;
public:
    ThreadsModel(wxDataViewListCtrl* view) : m_view(view){}
    virtual ~ThreadsModel() {}
    
    bool GetAttr(const wxDataViewItem& item, unsigned int col, wxDataViewItemAttr& attr) const {
        LLDBThreadViewClientData *cd = reinterpret_cast<LLDBThreadViewClientData*>(m_view->GetItemData(item));
        if ( cd && cd->GetThread().IsActive() ) {
            attr.SetBold(true);
            return true;
        }
        return false;
    }
};

//-------------------------------------------------------------
//-------------------------------------------------------------

class LLDBThreadsView : public LLDBThreadsViewBase
{
    LLDBPlugin *m_plugin;
    int m_selectedThread;
    wxObjectDataPtr<ThreadsModel> m_model;
    
public:
    LLDBThreadsView(wxWindow* parent, LLDBPlugin* plugin);
    virtual ~LLDBThreadsView();
private:
    void DoCleanup();
    
protected:
    virtual void OnItemActivated(wxDataViewEvent& event);
    
    void OnLLDBRunning(LLDBEvent &event);
    void OnLLDBStopped(LLDBEvent &event);
    void OnLLDBExited(LLDBEvent &event); 
    void OnLLDBStarted(LLDBEvent &event);

};
#endif // LLDBTHREADSVIEW_H
