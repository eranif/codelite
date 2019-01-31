#include "DirectoryChanger.h"
#include "allocator_mgr.h"
#include "designer_container_panel.h"
#include "designer_panel.h"
#include "gui.h"
#include "menu_bar.h"
#include "tool_bar.h"
#include "wxcTreeView.h"
#include "wxgui_helpers.h"
#include "wxguicraft_main_view.h"
#include <drawingutils.h>
#include <event_notifier.h>
#include <wx/app.h>
#include <wx/bookctrl.h>
#include <wx/choicebk.h>
#include <wx/clrpicker.h>
#include <wx/collpane.h>
#include <wx/combobox.h>
#include <wx/dataview.h>
#include <wx/dcbuffer.h>
#include <wx/dcscreen.h>
#include <wx/dirctrl.h>
#include <wx/filepicker.h>
#include <wx/gbsizer.h>
#include <wx/grid.h>
#include <wx/listbook.h>
#include <wx/listctrl.h>
#include <wx/log.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/notebook.h>
#include <wx/radiobox.h>
#include <wx/settings.h>
#include <wx/srchctrl.h>
#include <wx/statusbr.h>
#include <wx/toolbar.h>
#include <wx/toolbook.h>
#include <wx/treebook.h>
#include <wx/treelist.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

class BOOL_LOCKER
{
    bool& m_bool;

public:
    BOOL_LOCKER(bool& b)
        : m_bool(b)
    {
        m_bool = true;
    }
    ~BOOL_LOCKER() { m_bool = false; }
};

class CONTEXT_MENU
{
    bool m_fireEvent;

public:
    CONTEXT_MENU(bool fireEvent)
        : m_fireEvent(fireEvent)
    {
    }
    ~CONTEXT_MENU()
    {
        if(m_fireEvent) {
            wxCommandEvent contextMenuEvent(wxEVT_SHOW_CONTEXT_MENU);
            EventNotifier::Get()->AddPendingEvent(contextMenuEvent);
        }
    }
};

class HiddenFrame : public wxFrame
{
public:
    HiddenFrame()
        : wxFrame(NULL, wxID_ANY, wxT(""))
    {
        Hide();
    }
    virtual ~HiddenFrame() {}
};

const wxEventType wxEVT_PREVIEW_CTRL_SELECTED = wxNewEventType();
const wxEventType wxEVT_PREVIEW_BOOKPAGE_SELECTED = wxNewEventType();
const wxEventType wxEVT_PREVIEW_RIBBON_PAGE_SELECTED = wxNewEventType();

BEGIN_EVENT_TABLE(DesignerPanel, wxScrolledWindow)
EVT_AUINOTEBOOK_PAGE_CHANGED(wxID_ANY, DesignerPanel::OnAuiPageChanged)
EVT_AUINOTEBOOK_PAGE_CHANGING(wxID_ANY, DesignerPanel::OnAuiPageChanging)
EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, DesignerPanel::OnNotebookPageChanged)
EVT_LISTBOOK_PAGE_CHANGED(wxID_ANY, DesignerPanel::OnListbookPageChanged)
EVT_TOOLBOOK_PAGE_CHANGED(wxID_ANY, DesignerPanel::OnToolbookPageChanged)
EVT_CHOICEBOOK_PAGE_CHANGED(wxID_ANY, DesignerPanel::OnChoicebookPageChanged)
EVT_TREEBOOK_PAGE_CHANGED(wxID_ANY, DesignerPanel::OnTreebookPageChanged)
EVT_TOOL(wxID_ANY, DesignerPanel::OnAuiToolClicked)
EVT_RADIOBOX(wxID_ANY, DesignerPanel::OnRadioBox)
EVT_RIBBONBAR_PAGE_CHANGED(wxID_ANY, DesignerPanel::OnRibbonPageChanged)
EVT_SIZE(DesignerPanel::OnSize)
END_EVENT_TABLE()

DesignerPanel::DesignerPanel(wxWindow* parent, wxWindowID id, const wxPoint& position, const wxSize& size, long style)
    : wxScrolledWindow(parent, wxID_ANY, position, size, style)
    , m_constructing(true)
    , m_hintedWin(NULL)
    , m_parentWin(NULL)
    , m_hintedSizeritem(NULL)
    , m_hintedContainer(NULL)
{
    this->SetScrollRate(5, 5);
    wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
    SetSizer(sz);

    m_innerContainer = new DesignerContainerPanel(this);
    GetSizer()->Add(m_innerContainer, 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxALL, 5);

    wxColour bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
    if(DrawingUtils::IsDark(bgColour)) {
        bgColour = bgColour.ChangeLightness(120);
    } else {
        bgColour = bgColour.ChangeLightness(90);
    }
    SetBackgroundColour(bgColour);
    EventNotifier::Get()->Connect(wxEVT_UPDATE_PREVIEW, wxCommandEventHandler(DesignerPanel::OnUpdatePreview), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_WXGUI_PROJECT_CLOSED, wxCommandEventHandler(DesignerPanel::OnClearPreview),
                                  NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WXGUI_PROJECT_LOADED, wxCommandEventHandler(DesignerPanel::OnLoadPreview), NULL,
                                  this);
    EventNotifier::Get()->Connect(wxEVT_TREE_ITEM_SELECTED, wxCommandEventHandler(DesignerPanel::OnHighlightControl),
                                  NULL, this);
    GetSizer()->Layout();
}

DesignerPanel::~DesignerPanel()
{
    EventNotifier::Get()->Disconnect(wxEVT_UPDATE_PREVIEW, wxCommandEventHandler(DesignerPanel::OnUpdatePreview), NULL,
                                     this);
    EventNotifier::Get()->Disconnect(wxEVT_WXGUI_PROJECT_CLOSED, wxCommandEventHandler(DesignerPanel::OnClearPreview),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WXGUI_PROJECT_LOADED, wxCommandEventHandler(DesignerPanel::OnLoadPreview),
                                     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_TREE_ITEM_SELECTED, wxCommandEventHandler(DesignerPanel::OnHighlightControl),
                                     NULL, this);
}

void DesignerPanel::OnUpdatePreview(wxCommandEvent& e)
{
    e.Skip();

    BOOL_LOCKER bl(m_constructing);
    if(m_xrcLoaded == e.GetString()) return;

#ifdef __WXMSW__
    wxWindowUpdateLocker locker(this);
#endif

    if(m_innerContainer) { DoClear(); }

    m_xrcLoaded = e.GetString();
    DoLoadXRC(e.GetInt());
}

void DesignerPanel::OnClearPreview(wxCommandEvent& e)
{
    e.Skip();
#ifdef __WXMSW__
    wxWindowUpdateLocker locker(this);
#endif
    BOOL_LOCKER bl(m_constructing);
    DoClear();
}

void DesignerPanel::OnLoadPreview(wxCommandEvent& e)
{
    e.Skip();
#ifdef __WXMSW__
    wxWindowUpdateLocker locker(this);
#endif
    BOOL_LOCKER bl(m_constructing);
    DoClear();

    m_xrcLoaded = e.GetString();
    DoLoadXRC(e.GetInt());
}

void DesignerPanel::OnMouseLeftDown(wxMouseEvent& e) { DoControlSelected(e); }

void DesignerPanel::DoControlSelected(wxEvent& e)
{
    bool bIsRightUp = ::wxGetMouseState().RightIsDown();
    wxWindow* win = dynamic_cast<wxWindow*>(e.GetEventObject());
    if(win) {

        // Fire context menu if needed
        CONTEXT_MENU eventStarte(bIsRightUp);

        wxString clsname, parent_clsname;
        const wxClassInfo* clsInfo = win->GetClassInfo();
        if(clsInfo && clsInfo->GetClassName()) { clsname = clsInfo->GetClassName(); }

        if(win->GetParent() && win->GetParent()->GetClassInfo() && win->GetParent()->GetClassInfo()->GetClassName()) {
            parent_clsname = win->GetParent()->GetClassInfo()->GetClassName();
        }

        wxWindow* selectedWin = win;
        wxString thename = win->GetName();

        if(parent_clsname == wxT("wxGrid")) {
            selectedWin = win->GetParent();
            thename = win->GetParent()->GetName();

        } else if(parent_clsname == "OpenGLCanvas") {
            selectedWin = win->GetParent();
            thename = win->GetParent()->GetName();

        } else if(parent_clsname == "wxPropertyGridManager") {
            selectedWin = win->GetParent();
            thename = win->GetParent()->GetName();

        } else if(parent_clsname == wxT("wxListCtrl")) {
            selectedWin = win->GetParent();
            thename = win->GetParent()->GetName();

        } else if(parent_clsname == wxT("wxSearchCtrl")) {
            selectedWin = win->GetParent();
            thename = win->GetParent()->GetName();

        } else if(parent_clsname == wxT("wxColourPickerCtrl")) {
            selectedWin = win->GetParent();
            thename = win->GetParent()->GetName();

        } else if(parent_clsname == wxT("wxFilePickerCtrl")) {
            selectedWin = win->GetParent();
            thename = win->GetParent()->GetName();

        } else if(parent_clsname == wxT("wxDirPickerCtrl")) {
            selectedWin = win->GetParent();
            thename = win->GetParent()->GetName();

        } else if(parent_clsname == wxT("wxGenericDirCtrl")) {
            selectedWin = win->GetParent();
            thename = win->GetParent()->GetName();

        } else if(parent_clsname == "wxDataViewListCtrl") {
            selectedWin = win->GetParent();
            thename = win->GetParent()->GetName();

        } else if(parent_clsname == "wxDataViewTreeCtrl") {
            selectedWin = win->GetParent();
            thename = win->GetParent()->GetName();

        } else if(parent_clsname == "wxDataViewCtrl") {
            selectedWin = win->GetParent();
            thename = win->GetParent()->GetName();
        }

        // Allow click on wxNotebook to actually switch pages
        if(clsname == wxT("wxNotebook") || (parent_clsname == wxT("wxToolbook") && clsname == wxT("wxToolBar")) ||
           (parent_clsname == wxT("wxListbook") && clsname == wxT("wxListView")) ||
           (clsname == wxT("wxAuiTabCtrl") || parent_clsname == wxT("wxAuiNotebook")) ||
           (parent_clsname == wxT("wxTreebook") || clsname == "wxRibbonBar")) {
            e.Skip();
            DoDrawSurroundingMarker(selectedWin);
            return;
        }

        if(clsname == wxT("wxAuiToolBar")) {
            // We handle clicks in the OnAuiToolbarClicked() method
            e.Skip();
            DoDrawSurroundingMarker(selectedWin);
            return;
        }

        if(clsname == wxT("wxToolBar")) {
            e.Skip();
            DoDrawSurroundingMarker(selectedWin);
            return;
        }

        DoDrawSurroundingMarker(selectedWin);
        wxCommandEvent evt(wxEVT_PREVIEW_CTRL_SELECTED);
        evt.SetString(thename);
        EventNotifier::Get()->AddPendingEvent(evt);
    }
}

void DesignerPanel::RecurseConnectEvents(wxWindow* pclComponent)
{
    if(pclComponent) {
        wxString name = pclComponent->GetName();
        name.Trim();
        m_windows.insert(std::make_pair(name, pclComponent));
        pclComponent->Connect(wxID_ANY, wxEVT_LEFT_DOWN, wxMouseEventHandler(DesignerPanel::OnMouseLeftDown),
                              (wxObject*)NULL, this);

        pclComponent->Connect(wxID_ANY, wxEVT_RIGHT_DOWN, wxMouseEventHandler(DesignerPanel::OnMouseLeftDown),
                              (wxObject*)NULL, this);

        pclComponent->Connect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(DesignerPanel::OnControlFocus),
                              (wxObject*)NULL, this);
        // Now, for container windows e.g. panels, grab the opportunity to identify all contained sizers/spacers
        StoreSizersRecursively(pclComponent->GetSizer(),
                               pclComponent); // For non-containers this will harmlessly pass NULL

        wxWindowList::compatibility_iterator pclNode = pclComponent->GetChildren().GetFirst();
        while(pclNode) {
            wxWindow* pclChild = pclNode->GetData();
            this->RecurseConnectEvents(pclChild);

            pclNode = pclNode->GetNext();
        }
    }
}

void DesignerPanel::RecurseDisconnectEvents(wxWindow* pclComponent)
{
    if(pclComponent) {
        pclComponent->Disconnect(wxID_ANY, wxEVT_LEFT_DOWN, wxMouseEventHandler(DesignerPanel::OnMouseLeftDown),
                                 (wxObject*)NULL, this);

        pclComponent->Disconnect(wxID_ANY, wxEVT_RIGHT_DOWN, wxMouseEventHandler(DesignerPanel::OnMouseLeftDown),
                                 (wxObject*)NULL, this);

        pclComponent->Disconnect(wxID_ANY, wxEVT_SET_FOCUS, wxFocusEventHandler(DesignerPanel::OnControlFocus),
                                 (wxObject*)NULL, this);
        wxWindowList::compatibility_iterator pclNode = pclComponent->GetChildren().GetFirst();
        while(pclNode) {
            wxWindow* pclChild = pclNode->GetData();
            this->RecurseDisconnectEvents(pclChild);

            pclNode = pclNode->GetNext();
        }
    }
}

void DesignerPanel::StoreSizersRecursively(wxSizer* sizer, wxWindow* container)
{
    // There doesn't seem to be any way to retrieve a wxSizerItem for a top-level sizer :/
    // So ignore it (fortunately it won't have borders or an interesting outline) but add all children
    if(sizer) {
        wxSizerItemList::compatibility_iterator node = sizer->GetChildren().GetFirst();
        while(node) {
            wxSizerItem* child = node->GetData();
            // We need to process the contents of a staticboxsizer differently; they are positioned wrt the staticbox,
            // not the real container
            if(child->IsSizer()) {
                wxStaticBoxSizer* sbs = wxDynamicCast(child->GetSizer(), wxStaticBoxSizer);
                if(sbs) { container = (wxWindow*)(sbs->GetStaticBox()); }
            }

            int id = child->GetId(); // GetId() returns XRCID(name), or wxNOT_FOUND for ""
            if(id != wxNOT_FOUND) { m_sizeritems.insert(std::make_pair(id, SizeritemData(container, child))); }

            if(child->IsSizer()) {
                wxSizer* childsizer = child->GetSizer();
                StoreSizersRecursively(childsizer, container);
                if(!childsizer->GetItemCount()) {
                    // Empty sizers all get positioned at 0,0, which breaks outlining; they also annoy gtk.
                    // So let's pretend it contains a 5*5 spacer
                    // But be careful if it's a gridbagsizer: a plain AddSpacer() asserts
                    wxGridBagSizer* gbs = wxDynamicCast(childsizer, wxGridBagSizer);
                    if(gbs) {
                        gbs->Add(0, 0, wxGBPosition());
                    } else {
                        childsizer->AddSpacer(5);
                    }
                }
            }

            node = node->GetNext();
        }
    }
}

void DesignerPanel::DoMarkSizeritem(wxSizerItem* szitem, wxWindow* container)
{
    // If no item provided, remark the current selection
    if(!szitem && m_hintedContainer && m_hintedSizeritem) {
        wxClientDC dc(m_hintedContainer);
        dc.SetPen(wxColour("ORANGE"));
        wxRect rect(m_hintedSizeritem->GetRect());
        MarkOutline(dc, &rect);
        DoMarkBorders(dc, rect, m_hintedSizeritem->GetBorder(), m_hintedSizeritem->GetFlag());
        return;
    }

    ClearStaleOutlines();

    if(szitem && container) {
        m_hintedSizeritem = szitem;
        m_hintedContainer = container;
        wxClientDC dc(container);
        dc.SetPen(wxColour("ORANGE"));

        wxRect rect(m_hintedSizeritem->GetRect());
        MarkOutline(dc, &rect);
        DoMarkBorders(dc, rect, m_hintedSizeritem->GetBorder(), m_hintedSizeritem->GetFlag());
    }
}

void DesignerPanel::DoDrawSurroundingMarker(wxWindow* win)
{
    // NULL provided, remark the current selection
    if(!win && m_parentWin && m_hintedWin) {
        wxClientDC dc(m_parentWin);
        dc.SetPen(*wxRED_PEN);
        MarkOutline(dc);
        MarkBorders(dc);
        return;

    } else if(!win) {
        return;
    }

    wxWindow* parent = win->GetParent();
    if(!parent) { return; }

    ClearStaleOutlines();

    {
        m_hintedWin = win;
        m_parentWin = parent;

        wxClientDC dc(m_parentWin);
        dc.SetPen(*wxRED_PEN);
        MarkOutline(dc);
        MarkBorders(dc);
    }
}

void DesignerPanel::MarkOutline(wxDC& dc, wxRect* rect /*=NULL*/) const
{
    wxRect rr;
    if(!rect) {
        rr = m_hintedWin->GetRect();
    } else {
        rr = *rect;
    }
    const wxPoint pt = GetOutlineOffset();

    rr.Offset(pt);
    rr.Inflate(1, 1);
#if wxVERSION_NUMBER > 2904
    if(pt != wxPoint()) {
        // i.e. this is a notebookpage, so (for some reason) it needs to be more inflated
        rr.Inflate(2, 2);
    }
#endif

    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(rr);
}

void DesignerPanel::MarkBorders(wxDC& dc) const
{
    wxSizer* sizer = m_hintedWin->GetContainingSizer();
    if(!sizer) { return; }

    wxSizerItem* szitem = sizer->GetItem(m_hintedWin);
    if(!szitem || !szitem->GetBorder()) { return; }

    dc.SetBrush(*wxTRANSPARENT_BRUSH);

    DoMarkBorders(dc, m_hintedWin->GetRect(), szitem->GetBorder(), szitem->GetFlag());
}

void DesignerPanel::DoMarkBorders(wxDC& dc, wxRect rr, int bdrwidth, int flags) const
{
    if(flags & wxTOP) {
        wxPoint mid = rr.GetTopLeft() + wxPoint(rr.GetWidth() / 2, 0);
        dc.DrawLine(mid, mid - wxPoint(0, bdrwidth));
    }

    if(flags & wxBOTTOM) {
        wxPoint mid = rr.GetBottomLeft() + wxPoint(rr.GetWidth() / 2, 0);
        dc.DrawLine(mid, mid + wxPoint(0, bdrwidth));
    }

    if(flags & wxLEFT) {
        wxPoint mid = rr.GetTopLeft() + wxPoint(0, rr.GetHeight() / 2);
        dc.DrawLine(mid, mid - wxPoint(bdrwidth, 0));
    }

    if(flags & wxRIGHT) {
        wxPoint mid = rr.GetTopRight() + wxPoint(0, rr.GetHeight() / 2);
        dc.DrawLine(mid, mid + wxPoint(bdrwidth, 0));
    }
}

void DesignerPanel::ClearStaleOutlines() const
{
    // First any control
    if(m_parentWin && m_hintedWin) {
        wxClientDC dc(m_parentWin);
        dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
        MarkOutline(dc);
        MarkBorders(dc);
    }

    // Now any sizer/spacer
    if(m_hintedContainer && m_hintedSizeritem) {
        wxClientDC dc(m_hintedContainer);
        dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
        wxRect rect(m_hintedSizeritem->GetRect());
        MarkOutline(dc, &rect);
        DoMarkBorders(dc, rect, m_hintedSizeritem->GetBorder(), m_hintedSizeritem->GetFlag());
    }
}

wxPoint DesignerPanel::GetOutlineOffset() const
{
    wxPoint pt;
    if(m_parentWin && m_hintedWin && wxDynamicCast(m_parentWin, wxNotebook)) {
        int x, y, px, py;
        m_hintedWin->GetScreenPosition(&x, &y);
        m_parentWin->GetScreenPosition(&px, &py);
#if wxVERSION_NUMBER < 2905
        // If a wxNotebook has top or left tabs, the page outline needs offsetting to avoid them
        if(m_parentWin->GetWindowStyle() & wxNB_TOP) {
            pt.y = y - py;
        } else if(m_parentWin->GetWindowStyle() & wxNB_LEFT) {
            pt.x = x - px;
        }
#else
        // In 2.9.5 that's no longer true. However the small margin between the notebook and the page needs offsetting
        if(m_parentWin->GetWindowStyle() & wxNB_TOP) {
            pt.x = pt.y = px - x - 3; // The -3 is a kludge
        } else if(m_parentWin->GetWindowStyle() & wxNB_LEFT) {
            pt.x = pt.y = py - y - 3;
        }
#endif
    }
    return pt;
}

void DesignerPanel::DoClear()
{
    // Remove all children of this sizer
    m_innerContainer->Clear();
    m_xrcLoaded.Clear();
    m_hintedWin = NULL;
    m_parentWin = NULL;
    m_hintedSizeritem = NULL;
    m_hintedContainer = NULL;
    m_windows.clear();
}

void DesignerPanel::DoNotebookPageChangeEvent(wxEvent& e)
{
    if(m_constructing) { return; }

    wxBookCtrlBase* nbk = dynamic_cast<wxBookCtrlBase*>(e.GetEventObject());
    if(!nbk) { return; }

    int idx = nbk->GetSelection();
    if(idx != wxNOT_FOUND) {
        wxWindow* page = nbk->GetPage(idx);
        if(page) {
            wxCommandEvent evt(wxEVT_PREVIEW_BOOKPAGE_SELECTED);
            evt.SetString(page->GetName());
            EventNotifier::Get()->AddPendingEvent(evt);
        }
    }
}

void DesignerPanel::OnListbookPageChanged(wxListbookEvent& e) { DoNotebookPageChangeEvent(e); }

void DesignerPanel::OnNotebookPageChanged(wxNotebookEvent& e) { DoNotebookPageChangeEvent(e); }

void DesignerPanel::OnToolbookPageChanged(wxToolbookEvent& e) { DoNotebookPageChangeEvent(e); }

void DesignerPanel::OnChoicebookPageChanged(wxChoicebookEvent& e) { DoNotebookPageChangeEvent(e); }

void DesignerPanel::OnTreebookPageChanged(wxTreebookEvent& e) { DoNotebookPageChangeEvent(e); }

void DesignerPanel::DoAuiBookChanged(wxAuiNotebookEvent& e)
{
    if(m_constructing) { return; }

    wxAuiNotebook* nbk = dynamic_cast<wxAuiNotebook*>(e.GetEventObject());
    if(!nbk) return;

    int idx = nbk->GetSelection();
    if(idx != wxNOT_FOUND) {
        wxWindow* page = nbk->GetPage(idx);
        if(page) {
            wxCommandEvent evt(wxEVT_PREVIEW_BOOKPAGE_SELECTED);
            evt.SetString(page->GetName());
            EventNotifier::Get()->AddPendingEvent(evt);
        }
    }
}

void DesignerPanel::OnAuiPageChanged(wxAuiNotebookEvent& e) { DoAuiBookChanged(e); }

void DesignerPanel::OnAuiPageChanging(wxAuiNotebookEvent& e) { DoAuiBookChanged(e); }

void DesignerPanel::DoLoadXRC(int topLeveWinType)
{
    m_windows.clear();
    m_sizeritems.clear();
    wxPanel* panel = NULL;
    wxString caption;
    wxString styleString;
    wxString bmpIcon;
    wxFileName fn = wxCrafter::LoadXRC(m_xrcLoaded, wxT("DesignerPanel.xrc"), caption, styleString, bmpIcon);

    // We must be in the directory of the project to be able to load the bitmaps properly
    DirectoryChanger dc(wxcProjectMetadata::Get().GetProjectPath());
    panel = wxXmlResource::Get()->LoadPanel(m_innerContainer, wxT("PreviewPanel"));

    if(panel) {

        if(topLeveWinType == ID_WXFRAME || topLeveWinType == ID_WXDIALOG) {
            wxBitmap tlwIcon;
            if(!bmpIcon.IsEmpty()) {
                // convert bmpIcon to fullpath
                wxFileName fnBmp(bmpIcon);
                fnBmp.MakeAbsolute(wxcProjectMetadata::Get().GetProjectPath());
                tlwIcon = wxBitmap(fnBmp.GetFullPath(), wxBITMAP_TYPE_ANY);
            }
            m_innerContainer->EnableCaption(caption, styleString, tlwIcon);
        }

        RecurseConnectEvents(panel);
        wxMenuBar* mb = wxXmlResource::Get()->LoadMenuBar(wxT("MENU_BAR_ID"));
        if(mb) {
#ifdef __WXGTK__
            // GTK assumes that a wxMenuBar is attached to a frame
            // so give it one
            HiddenFrame* hf = new HiddenFrame();
            hf->SetMenuBar(mb);
#endif
            m_innerContainer->SetMenuBar(new MenuBar(m_innerContainer, mb));

#ifdef __WXGTK__
            hf->Destroy();
#endif
        }

        ToolBar* pane = new ToolBar(m_innerContainer);
        wxToolBar* tb = wxXmlResource::Get()->LoadToolBar(pane, wxT("TOOL_BAR_ID"));
        if(tb) {
            pane->AddToolbar(tb);
            m_innerContainer->SetToolbar(pane);

        } else {
            pane->Destroy();
        }
        panel->Show();
        m_innerContainer->AddMainView(panel);
        GetSizer()->Layout();
        wxStatusBar* sb = dynamic_cast<wxStatusBar*>(
            wxXmlResource::Get()->LoadObject(m_innerContainer, wxT("STATUS_BAR_ID"), wxT("wxStatusBar")));
        if(sb) { m_innerContainer->SetStatusBar(sb); }
        m_innerContainer->CalcBestSize(topLeveWinType);
        wxXmlResource::Get()->Unload(fn.GetFullPath());

        if(topLeveWinType == ID_WXWIZARD) { GetSizer()->Fit(m_innerContainer); }
    }
    Layout();
}

void DesignerPanel::OnAuiToolClicked(wxCommandEvent& e)
{
    wxAuiToolBar* tb = dynamic_cast<wxAuiToolBar*>(e.GetEventObject());
    wxToolBar* bar = dynamic_cast<wxToolBar*>(e.GetEventObject());
    if(tb) {

        wxAuiToolBarItem* tool = tb->FindTool(e.GetId());
        if(tool) {
            wxString label = tool->GetLabel();
            wxString s = tb->GetName();
            s << wxT(":") << label;
            wxCommandEvent evt(wxEVT_PREVIEW_BAR_SELECTED);
            evt.SetString(s);
            evt.SetInt(ID_WXAUITOOLBAR);
            EventNotifier::Get()->AddPendingEvent(evt);

        } else {
            e.Skip();
        }

    } else if(bar) {
        wxToolBarToolBase* tool = bar->FindById(e.GetId());
        if(tool) {
            wxString label = tool->GetLabel();
            wxString s;
            s << bar->GetName() << wxT(":") << label;
            wxCommandEvent evt(wxEVT_PREVIEW_BAR_SELECTED);
            evt.SetString(s);
            evt.SetInt(ID_WXTOOLBAR);
            EventNotifier::Get()->AddPendingEvent(evt);

        } else {
            e.Skip();
        }

    } else {
        e.Skip();
    }
}

void DesignerPanel::OnHighlightControl(wxCommandEvent& e)
{
    e.Skip();
    wxString s = e.GetString();

    if(m_windows.count(s)) {
        DoDrawSurroundingMarker(m_windows[s]);
    } else {
        // If it wasn't in the window list, it should be a sizer or spacer
        int id = wxXmlResource::GetXRCID(s);
        if((id != wxID_NONE) && m_sizeritems.count(id)) {
            SizeritemData* sid = &(m_sizeritems.find(id)->second);
            DoMarkSizeritem(sid->m_sizeritem, sid->m_parentWin);
        }
    }
}

void DesignerPanel::OnControlFocus(wxFocusEvent& event)
{
    event.Skip();
#if defined(__WXGTK__) || defined(__WXMAC__)
    wxDataViewTreeCtrl* win1 = dynamic_cast<wxDataViewTreeCtrl*>(event.GetEventObject());
    wxDataViewCtrl* win2 = dynamic_cast<wxDataViewCtrl*>(event.GetEventObject());
    wxDataViewListCtrl* win3 = dynamic_cast<wxDataViewListCtrl*>(event.GetEventObject());
    if(win1 || win2 || win3) { DoControlSelected(event); }
#endif
}

void DesignerPanel::OnRadioBox(wxCommandEvent& e)
{
    e.Skip();
    wxRadioBox* rb = dynamic_cast<wxRadioBox*>(e.GetEventObject());
    if(rb) { DoControlSelected(e); }
}

void DesignerPanel::OnRibbonPageChanged(wxRibbonBarEvent& e)
{
    e.Skip();
    if(e.GetPage()) {
        wxCommandEvent evt(wxEVT_PREVIEW_RIBBON_PAGE_SELECTED);
        evt.SetString(e.GetPage()->GetName());
        EventNotifier::Get()->AddPendingEvent(evt);
    }
}

void DesignerPanel::OnTreeListCtrlFocus(wxFocusEvent& e)
{
    e.Skip();
    wxTreeListCtrl* rb = dynamic_cast<wxTreeListCtrl*>(e.GetEventObject());
    if(rb) { DoControlSelected(e); }
}

void DesignerPanel::OnSize(wxSizeEvent& event)
{
    event.Skip();
    Refresh();
}
