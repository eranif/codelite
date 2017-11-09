#ifndef CLEDITORBAR_H
#define CLEDITORBAR_H
#include "drawingutils.h"
#include "wxcrafter_plugin.h"
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

    // Breadcrumbs
    wxString m_filename;
    wxString m_filenameRelative;
    wxString m_projectFile;
    wxString m_projectName;
    wxArrayString m_breadcrumbs;

    wxRect m_filenameRect;
    eButtonState m_state;

private:
    void DoRefreshColoursAndFonts();

public:
    clEditorBar(wxWindow* parent);
    virtual ~clEditorBar();
    void SetMessage(const wxString& className, const wxString& function);
    void DoShow(bool s);

protected:
    virtual void OnEditorSize(wxSizeEvent& e);
    virtual void OnEraseBG(wxEraseEvent& e);
    virtual void OnPaint(wxPaintEvent& e);
    void OnEditorChanged(wxCommandEvent& e);
    void OnLeftDown(wxMouseEvent& e);
    void OnLeftUp(wxMouseEvent& e);
    void OnThemeChanged(wxCommandEvent& e);
    void OnEnterWindow(wxMouseEvent& e);
    void OnLeaveWindow(wxMouseEvent& e);
};
#endif // CLEDITORBAR_H
