#ifndef CLEDITORBAR_H
#define CLEDITORBAR_H

#include "drawingutils.h"
#include "wxcrafter_plugin.h"
#include <vector>
#include <wx/colour.h>
#include <wx/font.h>

class WXDLLIMPEXP_SDK clEditorBar : public clEditorBarBase
{
    wxString m_classname;
    wxString m_function;
    wxColour m_functionColour;
    wxColour m_classColour;
    wxColour m_defaultColour;
    wxColour m_bgColour;
    wxFont m_textFont;
    wxRect m_scopeRect;
    eButtonState m_scopeButtonState;
    wxBitmap m_functionBmp;
    
    // Breadcrumbs
    wxString m_filename;
    wxString m_filenameRelative;
    wxString m_projectFile;
    wxString m_projectName;
    wxArrayString m_breadcrumbs;

    wxRect m_filenameRect;
    eButtonState m_state;

    // Bookmarks button
    wxBitmap m_bookmarksBmp;
    wxRect m_bookmarksRect;
    eButtonState m_bookmarksButtonState;
    std::vector<std::pair<int, wxString> > m_bookmarks;

private:
    void DoRefreshColoursAndFonts();
    void DoRefresh();
    void CreateBookmarksBitmap();

public:
    clEditorBar(wxWindow* parent);
    virtual ~clEditorBar();
    void SetMessage(const wxString& className, const wxString& function);
    void DoShow(bool s);

protected:
    void OnMarkerChanged(clCommandEvent& event);
    void OnEditorSize(wxSizeEvent& e);
    void OnEraseBG(wxEraseEvent& e);
    void OnPaint(wxPaintEvent& e);
    void OnEditorChanged(wxCommandEvent& e);
    void OnLeftDown(wxMouseEvent& e);
    void OnLeftUp(wxMouseEvent& e);
    void OnThemeChanged(wxCommandEvent& e);
    void OnIdle(wxIdleEvent& event);
};
#endif // CLEDITORBAR_H
