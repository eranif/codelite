#include "DebuggerToolBar.h"

#include "bitmap_loader.h"
#include "clToolBar.h"
#include "cl_config.h"
#include "drawingutils.h"
#include "editor_config.h"
#include "event_notifier.h"
#include "globals.h"
#include "imanager.h"
#include "windowattrmanager.h"

#include <wx/dcbuffer.h>
#include <wx/frame.h>
#include <wx/xrc/xmlres.h>

DebuggerToolBar::DebuggerToolBar(wxWindow* parent)
{
    if(!wxPanel::Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)) {
        return;
    }
    Hide();
    SetSizer(new wxBoxSizer(wxVERTICAL));
    wxPanel* mainPanel = new wxPanel(this);
    mainPanel->SetSizer(new wxBoxSizer(wxVERTICAL));
    m_gripper = new Gripper(mainPanel);
    mainPanel->GetSizer()->Add(m_gripper, 0, wxEXPAND);
    GetSizer()->Add(mainPanel, 1, wxEXPAND);

    // Create the toolbar itself
    //===-----------------------------------------------------------------------------
    // Debugger floating toolbar
    // The rest of the debugger buttons will go to a separate floating toolbar
    //===-----------------------------------------------------------------------------
    m_tb = new clToolBarGeneric(mainPanel);
    m_tb->SetMiniToolBar(true);

    clBitmapList* images = new clBitmapList;
    m_tb->AddTool(XRCID("start_debugger"), _("Continue"), images->Add("debugger_start"), _("Continue"));
    m_tb->AddTool(XRCID("stop_debugger"), _("Stop debugger"), images->Add("debugger_stop"), _("Stop debugger"));
    m_tb->AddTool(XRCID("pause_debugger"), _("Pause debugger"), images->Add("interrupt"), _("Pause debugger"));
    m_tb->AddTool(XRCID("restart_debugger"), _("Restart debugger"), images->Add("debugger_restart"),
                  _("Restart debugger"));
    m_tb->AddTool(XRCID("show_cursor"), _("Show Current Line"), images->Add("show_current_line"),
                  _("Show Current Line"));
    m_tb->AddTool(XRCID("dbg_stepin"), _("Step Into"), images->Add("step_in"), _("Step In"));
    m_tb->AddTool(XRCID("dbg_next"), _("Next"), images->Add("next"), _("Next"));
    m_tb->AddTool(XRCID("dbg_stepout"), _("Step Out"), images->Add("step_out"), _("Step Out"));
    m_tb->AddSeparator();
    m_tb->AddTool(XRCID("dbg_enable_reverse_debug"), _("Toggle Rewind Commands"), images->Add("rewind"),
                  _("Toggle Rewind Commands"), wxITEM_CHECK);
    m_tb->AddTool(XRCID("dbg_start_recording"), _("Start Reverse Debug Recording"), images->Add("record"),
                  _("Start Reverse Debug Recording"), wxITEM_CHECK);
    m_tb->AddSeparator();
    m_tb->AssignBitmaps(images);
    m_tb->Realize();
    // Let the top level window (i.e. the main frame) process this
    m_tb->Bind(wxEVT_TOOL, [&](wxCommandEvent& event) {
        wxFrame* topLevel = EventNotifier::Get()->TopFrame();
        topLevel->GetEventHandler()->AddPendingEvent(event);
    });
    mainPanel->GetSizer()->Add(m_tb, 0, wxEXPAND);
    // wxSize tbSize = m_tb->GetSize();
    GetSizer()->Fit(this);
}

DebuggerToolBar::~DebuggerToolBar() {}

bool DebuggerToolBar::Show(bool show)
{
    bool res = wxWindow::Show(show);
    if(show) {
        int x = clConfig::Get().Read("DebuggerToolBar/x", wxNOT_FOUND);
        if(x != wxNOT_FOUND) {
            Move(x, 0);
        } else {
            CentreOnParent(wxHORIZONTAL);
        }
    } else {
        clConfig::Get().Write("DebuggerToolBar/x", GetPosition().x);
    }
    return res;
}

Gripper::Gripper(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxNO_BORDER)
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    Bind(wxEVT_LEFT_DOWN, &Gripper::OnLeftDown, this);
    Bind(wxEVT_LEFT_UP, &Gripper::OnLeftUp, this);
    Bind(wxEVT_MOTION, &Gripper::OnMotion, this);
    Bind(wxEVT_PAINT, &Gripper::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {});
    wxTheApp->Bind(wxEVT_ACTIVATE_APP, &Gripper::OnAppActivated, this);
    // SetSizeHints(wxNOT_FOUND, 10);
    SetSizeHints(wxNOT_FOUND, 0);
}

Gripper::~Gripper()
{
    Unbind(wxEVT_LEFT_DOWN, &Gripper::OnLeftDown, this);
    Unbind(wxEVT_LEFT_UP, &Gripper::OnLeftUp, this);
    Unbind(wxEVT_MOTION, &Gripper::OnMotion, this);
    Unbind(wxEVT_PAINT, &Gripper::OnPaint, this);
    wxTheApp->Unbind(wxEVT_ACTIVATE_APP, &Gripper::OnAppActivated, this);
    // if(HasCapture()) { ReleaseMouse(); }
}

void Gripper::OnAppActivated(wxActivateEvent& event)
{
    event.Skip();
    //    if(!event.GetActive()) {
    //        // App being de-activated, release the mouse
    //        if(HasCapture()) { ReleaseMouse(); }
    //    }
}

void Gripper::OnLeftDown(wxMouseEvent& event)
{
    event.Skip();
    //    CaptureMouse();
    //    m_offset.x = event.GetPosition().x - GetPosition().x;
    //    m_offset.y = 0;
}

void Gripper::OnLeftUp(wxMouseEvent& event)
{
    event.Skip();
    // if(HasCapture()) { ReleaseMouse(); }
}

void Gripper::OnMotion(wxMouseEvent& event)
{
    event.Skip();
    //    if(HasCapture()) {
    //        wxPoint newPos = event.GetPosition();
    //        newPos.x -= m_offset.x;
    //        newPos.y -= m_offset.y;
    //        GetGrandParent()->Move(newPos);
    //    }
}

void Gripper::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    // DrawingUtils::DrawStippleBackground(GetClientRect(), dc);
    dc.SetPen(DrawingUtils::GetPanelBgColour());
    dc.SetBrush(DrawingUtils::GetPanelBgColour());
    dc.DrawRectangle(GetClientRect());
}
