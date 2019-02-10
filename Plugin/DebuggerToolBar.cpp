#include "DebuggerToolBar.h"
#include "globals.h"
#include "imanager.h"
#include "bitmap_loader.h"
#include "windowattrmanager.h"
#include <wx/xrc/xmlres.h>
#include "clToolBar.h"
#include "editor_config.h"
#include "cl_config.h"
#include <wx/dcbuffer.h>
#include "drawingutils.h"
#include "event_notifier.h"
#include <wx/frame.h>

DebuggerToolBar::DebuggerToolBar(wxWindow* parent)
{
    if(!wxPanel::Create(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE)) { return; }
    Hide();
    SetSizer(new wxBoxSizer(wxVERTICAL));
    wxPanel* mainPanel = new wxPanel(this);
    mainPanel->SetSizer(new wxBoxSizer(wxVERTICAL));
    m_gripper = new Gripper(mainPanel);
    mainPanel->GetSizer()->Add(m_gripper, 0, wxEXPAND);
    GetSizer()->Add(mainPanel, 1, wxEXPAND);

    OptionsConfigPtr options = EditorConfigST::Get()->GetOptions();
    int toolSize = options->GetIconsSize();

    // Create the toolbar itself
    //===-----------------------------------------------------------------------------
    // Debugger floating toolbar
    // The rest of the debugger buttons will go to a separate floating toolbar
    //===-----------------------------------------------------------------------------
    BitmapLoader& bmpLoader = *clGetManager()->GetStdIcons();
    m_tb = new clToolBar(mainPanel);
    m_tb->SetMiniToolBar(false);
    
    m_tb->AddTool(XRCID("start_debugger"), _("Continue"), bmpLoader.LoadBitmap(wxT("debugger_start"), toolSize),
                  _("Continue"));
    m_tb->AddTool(XRCID("stop_debugger"), _("Stop debugger"), bmpLoader.LoadBitmap(wxT("debugger_stop"), toolSize),
                  _("Stop debugger"));
    m_tb->AddTool(XRCID("pause_debugger"), _("Pause debugger"), bmpLoader.LoadBitmap(wxT("interrupt"), toolSize),
                  _("Pause debugger"));
    m_tb->AddTool(XRCID("restart_debugger"), _("Restart debugger"),
                  bmpLoader.LoadBitmap(wxT("debugger_restart"), toolSize), _("Restart debugger"));
    m_tb->AddTool(XRCID("show_cursor"), _("Show Current Line"),
                  bmpLoader.LoadBitmap(wxT("show_current_line"), toolSize), _("Show Current Line"));
    m_tb->AddTool(XRCID("dbg_stepin"), _("Step Into"), bmpLoader.LoadBitmap(wxT("step_in"), toolSize), _("Step In"));
    m_tb->AddTool(XRCID("dbg_next"), _("Next"), bmpLoader.LoadBitmap(wxT("next"), toolSize), _("Next"));
    m_tb->AddTool(XRCID("dbg_stepout"), _("Step Out"), bmpLoader.LoadBitmap(wxT("step_out"), toolSize), _("Step Out"));
    m_tb->AddSeparator();
    m_tb->AddTool(XRCID("dbg_enable_reverse_debug"), _("Toggle Rewind Commands"),
                  bmpLoader.LoadBitmap("rewind", toolSize), _("Toggle Rewind Commands"), wxITEM_CHECK);
    m_tb->AddTool(XRCID("dbg_start_recording"), _("Start Reverse Debug Recording"),
                  bmpLoader.LoadBitmap("record", toolSize), _("Start Reverse Debug Recording"), wxITEM_CHECK);
    m_tb->AddSeparator();
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
    //SetSizeHints(wxNOT_FOUND, 10);
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
