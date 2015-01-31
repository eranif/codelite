#ifndef CLSTATUSBAR_H
#define CLSTATUSBAR_H

#include "wxCustomStatusBar.h" // Base class: wxCustomStatusBar
#include "codelite_exports.h"

class IManager;
class WXDLLIMPEXP_SDK clStatusBar : public wxCustomStatusBar
{
    IManager* m_mgr;
protected:
    void OnUpdateColour(wxCommandEvent& event);

public:
    clStatusBar(wxWindow* parent, IManager* mgr);
    virtual ~clStatusBar();

    void SetMessage(const wxString& message, int col);
};

#endif // CLSTATUSBAR_H
