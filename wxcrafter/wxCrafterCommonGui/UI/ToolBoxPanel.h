#ifndef TOOLBOXPANEL_H
#define TOOLBOXPANEL_H

#include <wx/aui/auibook.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statline.h>

class GUICraftMainPanel;
class clCommandEvent;

/// The palette of the controls: a notebook (wxAuiNotebook with the tab art of CodeLite), a page for each kind of
/// control (forms, sizers, ...) with a button for each control. The pages are described by a table (see
/// ToolBoxPanel.cpp). A click on a button is handled by the main view (the ID of the button is the ID of the control).
class ToolBoxPanel : public wxPanel
{
public:
    ToolBoxPanel(wxWindow* parent, GUICraftMainPanel* mainView);
    ~ToolBoxPanel() override;

protected:
    /// The colours or the fonts of the editor changed: update the tab art
    void OnEditorColoursUpdated(clCommandEvent& event);
    /// Enable the button only if its control can be added to the selected item
    void OnControlUI(wxUpdateUIEvent& event);

    GUICraftMainPanel* m_mainView;
    wxAuiNotebook* m_notebook = nullptr;
};
#endif // TOOLBOXPANEL_H
