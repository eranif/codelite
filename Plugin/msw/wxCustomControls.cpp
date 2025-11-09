#include "wxCustomControls.hpp"

int clMessageBox(const wxString& message, const wxString& caption, long style, wxWindow* parent, int x, int y)
{
    wxGenericMessageDialog dlg{parent, message, caption, style, wxPoint{x, y}};
    dlg.ShowModal();

    // Map button ID to answer
    switch (dlg.GetReturnCode()) {
    case wxID_YES:
        return wxYES;
    case wxID_NO:
        return wxNO;
    case wxID_CANCEL:
        return wxCANCEL;
    default:
        return wxCANCEL;
    }
}
