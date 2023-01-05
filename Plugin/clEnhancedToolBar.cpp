#include "clEnhancedToolBar.hpp"

#include "clToolBarButtonBase.h"
#include "file_logger.h"

clEnhancedToolBar::clEnhancedToolBar(wxWindow* parent, wxWindowID winid, const wxPoint& pos, const wxSize& size,
                                     long style, const wxString& name)
    : clToolBarGeneric(parent, winid, pos, size, style, name)
{
}

clEnhancedToolBar::~clEnhancedToolBar() {}

void clEnhancedToolBar::Add2StatesTool(wxWindowID id, wxEvtHandler* sink, const Button& button1, const Button& button2,
                                       wxItemKind kind)
{
    // we add this tool with the initial setup of button1
    m_buttons.insert({ id, { button1, button2, 0, sink } });
    AddTool(id, button1.label, button1.bmp_id, button1.label, kind);
    Bind(wxEVT_TOOL, &clEnhancedToolBar::OnButtonClicked, this, id);
}

void clEnhancedToolBar::OnButtonClicked(wxCommandEvent& event)
{
    clDEBUG() << "Button" << event.GetId() << "clicked";
    auto where = m_buttons.find(event.GetId());
    if(where == m_buttons.end()) {
        return;
    }
    auto& state = m_buttons[event.GetId()];
    const Button& currentSelection = state.GetSelection();

    // Pick the sink object
    wxEvtHandler* sink = state.GetSink(this);

    // First the event for the current selection
    wxCommandEvent e(wxEVT_TOOL, currentSelection.id);
    e.SetEventObject(this);
    sink->AddPendingEvent(e);
}

void clEnhancedToolBar::SetButtonAction(wxWindowID buttonId, wxWindowID actionID)
{
    auto where = m_buttons.find(buttonId);
    if(where == m_buttons.end()) {
        clDEBUG() << "Could not find button with id" << buttonId;
        return;
    }
    auto& state = m_buttons[buttonId];
    auto button = FindById(buttonId);
    if(!button) {
        return;
    }
    Button* btn = nullptr;
    if(state.button1.id == actionID) {
        btn = &state.button1;
        state.selection = 0;
    } else {
        btn = &state.button2;
        state.selection = 1;
    }
    button->SetBitmapIndex(btn->bmp_id);
    button->SetLabel(btn->label);
    LOG_IF_TRACE { clDEBUG1() << "toolbar button changed state to" << btn->label; }
    Refresh();
}
