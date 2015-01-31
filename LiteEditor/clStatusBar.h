#ifndef CLSTATUSBAR_H
#define CLSTATUSBAR_H

#include "wxCustomStatusBar.h" // Base class: wxCustomStatusBar

class IManager;
class clStatusBar : public wxCustomStatusBar
{
    IManager* m_mgr;
protected:
    void OnUpdateColour(wxCommandEvent& event);

public:
    clStatusBar(wxWindow* parent, wxWindowID id = wxID_ANY);
    virtual ~clStatusBar();

    void SetMessage(const wxString& message, int col);
};

#endif // CLSTATUSBAR_H
