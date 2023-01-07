#ifndef __designer_panel__
#define __designer_panel__

#include "menu_bar.h"
#include <map>
#include <wx/aui/auibook.h>
#include <wx/choicebk.h>
#include <wx/collpane.h>
#include <wx/listbook.h>
#include <wx/notebook.h>
#include <wx/ribbon/bar.h>
#include <wx/scrolwin.h>
#include <wx/toolbook.h>
#include <wx/treebook.h>

class DesignerContainerPanel;
class HintFrame;

struct SizeritemData {
    SizeritemData(wxWindow* win, wxSizerItem* item)
        : m_parentWin(win)
        , m_sizeritem(item)
    {
    }
    wxWindow* m_parentWin;
    wxSizerItem* m_sizeritem;
};

class DesignerPanel : public wxScrolledWindow
{
    bool m_constructing;
    wxString m_xrcLoaded;
    DesignerContainerPanel* m_mainPanel = nullptr;
    wxWindow* m_hintedWin;
    wxWindow* m_parentWin;
    wxSizerItem* m_hintedSizeritem;
    wxWindow* m_hintedContainer;
    wxString m_selecteItem;
    std::map<wxString, wxWindow*> m_windows;
    std::map<int, SizeritemData> m_sizeritems;

protected:
    void RecurseConnectEvents(wxWindow* pclComponent);
    void RecurseDisconnectEvents(wxWindow* pclComponent);
    void StoreSizersRecursively(wxSizer* sizer, wxWindow* container);
    void DoDrawSurroundingMarker(wxWindow* win);
    void DoMarkSizeritem(wxSizerItem* szitem, wxWindow* container);
    void ClearStaleOutlines() const;
    void MarkOutline(wxDC& dc, wxRect* rect = NULL) const;
    void MarkBorders(wxDC& dc) const;
    void DoMarkBorders(wxDC& dc, wxRect rr, int bdrwidth, int flags) const;
    wxPoint GetOutlineOffset() const; // Needed for pages in a wxNotebook with top/left tabs
    void DoNotebookPageChangeEvent(wxEvent& e);
    void DoAuiBookChanged(wxAuiNotebookEvent& e);
    void DoClear();
    void DoLoadXRC(int topLeveWinType);
    void DoControlSelected(wxEvent& e);

public:
    void OnTreeListCtrlFocus(wxFocusEvent& e);

public:
    DesignerPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& position = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~DesignerPanel();

    DECLARE_EVENT_TABLE()
    void OnUpdatePreview(wxCommandEvent& e);
    void OnClearPreview(wxCommandEvent& e);
    void OnLoadPreview(wxCommandEvent& e);
    void OnMouseLeftDown(wxMouseEvent& e);
    void OnControlFocus(wxFocusEvent& event);
    void OnNotebookPageChanged(wxNotebookEvent& e);
    void OnListbookPageChanged(wxListbookEvent& e);
    void OnToolbookPageChanged(wxToolbookEvent& e);
    void OnChoicebookPageChanged(wxChoicebookEvent& e);
    void OnTreebookPageChanged(wxTreebookEvent& e);
    void OnAuiPageChanged(wxAuiNotebookEvent& e);
    void OnAuiPageChanging(wxAuiNotebookEvent& e);
    void OnAuiToolClicked(wxCommandEvent& e);
    void OnHighlightControl(wxCommandEvent& e);
    void OnRadioBox(wxCommandEvent& e);
    void OnRibbonPageChanged(wxRibbonBarEvent& e);
    void OnSize(wxSizeEvent& event);
};

#endif // __designer_panel__
