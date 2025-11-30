#include "outline_tab.h"

#include "LSP/LSPManager.hpp"
#include "clSideBarCtrl.hpp"
#include "codelite_events.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"


using namespace LSP;

namespace {
    class wxIntClientData : public wxClientData {
        int m_value = 0;
    public: 
        wxIntClientData(int value) : wxClientData(), m_value(value) {}
        ~wxIntClientData() = default;
        
        int GetValue() const { return m_value; }
        void SetValue(int value) { m_value = value; }
    };
}


OutlineTab::OutlineTab(wxWindow* parent)
    : LSPOutlineView(parent, wxID_ANY, LSPOutlineView::STYLE_NO_SEARCHBAR | LSPOutlineView::STYLE_SMALL_FONT)
{
    EventNotifier::Get()->Bind(wxEVT_LSP_DOCUMENT_SYMBOLS_REQUESTED, &OutlineTab::OnSymbolsRequested, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW, &OutlineTab::OnOutlineSymbols, this);
    EventNotifier::Get()->Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &OutlineTab::OnActiveEditorChanged, this);
    EventNotifier::Get()->Bind(wxEVT_ALL_EDITORS_CLOSED, &OutlineTab::OnAllEditorsClosed, this);
    EventNotifier::Get()->Bind(wxEVT_SIDEBAR_SELECTION_CHANGED, &OutlineTab::OnSideBarPageChanged, this);
     
    Bind(wxEVT_LSP_OUTLINE_SYMBOL_ACTIVATED, [&](wxCommandEvent& event) 
        {        
            IEditor* active_editor = clGetManager()->GetActiveEditor();
            active_editor->GetCtrl()->CallAfter(&wxStyledTextCtrl::SetFocus);
        } 
    );
}


OutlineTab::~OutlineTab()
{
    EventNotifier::Get()->Unbind(wxEVT_LSP_DOCUMENT_SYMBOLS_REQUESTED, &OutlineTab::OnSymbolsRequested, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_DOCUMENT_SYMBOLS_OUTLINE_VIEW, &OutlineTab::OnOutlineSymbols, this);
    EventNotifier::Get()->Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &OutlineTab::OnActiveEditorChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_ALL_EDITORS_CLOSED, &OutlineTab::OnAllEditorsClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_SIDEBAR_SELECTION_CHANGED, &OutlineTab::OnSideBarPageChanged, this);
}


void OutlineTab::OnOutlineSymbols(LSPEvent& event)
{
    event.Skip();
    if (!IsShown()) {
        return;
    }
    
    // depending on what data we get, show a different view
    if (!event.GetDocumentSymbols().empty()) {        
        SetSymbols(event.GetDocumentSymbols(), event.GetFileName());
    }
    else if (!event.GetSymbolsInformation().empty()) {
        SetSymbols(event.GetSymbolsInformation(), event.GetFileName());        
    }
    else {
        SetEmptySymbols();
    }
}


void OutlineTab::OnSymbolsRequested(LSPEvent& event) 
{
    event.Skip();
    if (!IsShown()) {
        return;
    }
    SetEmptySymbols();
}


void OutlineTab::OnAllEditorsClosed(wxCommandEvent& event)
{
    event.Skip();
    ClearView();
}


void OutlineTab::OnActiveEditorChanged(wxCommandEvent& event)
{
    event.Skip();
    ClearView();
    SetEmptySymbols();
}


void OutlineTab::OnSideBarPageChanged(clCommandEvent& event)
{
    LSP_DEBUG() << "OutlineTab::OnSideBarPageChanged: Requesting Symbols" << endl;
    event.Skip();
    // Clear and request symbols for the current editor.
    ClearView();
    SetEmptySymbols();
    IEditor* active_editor = clGetManager()->GetActiveEditor();
    LSP::Manager::GetInstance().RequestSymbolsForEditor(active_editor, nullptr);
}