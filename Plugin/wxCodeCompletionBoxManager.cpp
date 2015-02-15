#include "wxCodeCompletionBoxManager.h"
#include <wx/app.h>

wxCodeCompletionBoxManager::wxCodeCompletionBoxManager()
    : m_box(NULL)
{
}

wxCodeCompletionBoxManager::~wxCodeCompletionBoxManager() {}

wxCodeCompletionBoxManager& wxCodeCompletionBoxManager::Get()
{
    static wxCodeCompletionBoxManager manager;
    return manager;
}

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const TagEntryPtrVector_t& tags,
                                                   wxEvtHandler* eventObject)
{
    if(m_box) {
        m_box->Destroy();
    }

    m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    m_box->ShowCompletionBox(ctrl, tags);
}

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const wxCodeCompletionBoxEntry::Vec_t& entries,
                                                   wxEvtHandler* eventObject)
{
    if(m_box) {
        m_box->Destroy();
    }
    m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    m_box->ShowCompletionBox(ctrl, entries);
}

void wxCodeCompletionBoxManager::WindowDestroyed() { m_box = NULL; }

void wxCodeCompletionBoxManager::ShowCompletionBox(wxStyledTextCtrl* ctrl,
                                                   const wxCodeCompletionBoxEntry::Vec_t& entries,
                                                   const wxCodeCompletionBox::BmpVec_t& bitmaps,
                                                   wxEvtHandler* eventObject)
{
    if(m_box) {
        m_box->Destroy();
    }
    m_box = new wxCodeCompletionBox(wxTheApp->GetTopWindow(), eventObject);
    m_box->SetBitmaps(bitmaps);
    m_box->ShowCompletionBox(ctrl, entries);
}
