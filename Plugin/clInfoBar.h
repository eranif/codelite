#ifndef CLINFOBAR_H
#define CLINFOBAR_H

#include <wx/infobar.h>
#include <vector>
#include "codelite_exports.h"

class WXDLLIMPEXP_SDK clInfoBar : public wxInfoBar
{
    std::vector<std::pair<wxWindowID, wxString> > m_buttons;

protected:
    void Clear();

public:
    clInfoBar(wxWindow* parent, wxWindowID winid = wxID_ANY);
    virtual ~clInfoBar();

    void DisplayMessage(const wxString& message, int flags = wxICON_INFORMATION,
                        const std::vector<std::pair<wxWindowID, wxString> >& buttons = {});
};

#endif // CLINFOBAR_H
