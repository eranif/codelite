#ifndef DESIGNERCONTAINERPANEL_H
#define DESIGNERCONTAINERPANEL_H

#include "menu_bar.h"
#include "tool_bar.h"
#include <set>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/statusbr.h>

class CaptionBar;
class DesignerContainerPanel : public wxPanel
{
    wxPanel* m_mainPanel;
    int m_height;
    int m_width;
    std::set<wxObject*> m_windows;
    int m_topLevelType;
    CaptionBar* m_caption;

protected:
    void DoConnectCharEvent(wxWindow* win);

protected:
    void OnLeftDown(wxMouseEvent& e);
    void OnRightDown(wxMouseEvent& e);
    void OnSize(wxSizeEvent& event);

public:
    DesignerContainerPanel(wxWindow* parent);
    virtual ~DesignerContainerPanel();

    void AddMainView(wxPanel* panel);
    wxPanel* GetMainPanel() const { return m_mainPanel; }
    void SetToolbar(ToolBar* tb);
    void SetMenuBar(MenuBar* mb);
    void EnableCaption(const wxString& caption, const wxString& style, const wxBitmap& icon);
    void SetStatusBar(wxStatusBar* mb);
    void CalcBestSize(int winType);
};

#endif // DESIGNERCONTAINERPANEL_H
