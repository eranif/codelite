#ifndef CLEDITORBAR_H
#define CLEDITORBAR_H
#include "wxcrafter_plugin.h"
#include <wx/font.h>
#include <wx/colour.h>

class WXDLLIMPEXP_SDK clEditorBar : public clEditorBarBase
{
    wxString m_classname;
    wxString m_function;
    wxColour m_functionColour;
    wxColour m_classColour;
    wxColour m_defaultColour;
    wxColour m_bgColour;
    wxFont m_textFont;

private:
    void DoRefreshColoursAndFonts();

public:
    clEditorBar(wxWindow* parent);
    virtual ~clEditorBar();
    void SetMessage(const wxString& className, const wxString& function);
    void DoShow(bool s);

protected:
    virtual void OnEraseBG(wxEraseEvent& event);
    virtual void OnPaint(wxPaintEvent& event);
    void OnEditorChanged(wxCommandEvent& event);
    void OnThemeChanged(wxCommandEvent& event);
};
#endif // CLEDITORBAR_H
