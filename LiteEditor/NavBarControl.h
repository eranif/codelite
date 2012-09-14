#ifndef NAVBARCONTROL_H
#define NAVBARCONTROL_H
#include "wxcrafter.h"

class NavBarControl : public NavBarControlBaseClass
{
public:
    NavBarControl(wxWindow* parent);
    virtual ~NavBarControl();
protected:
    virtual void OnFuncListMouseDown(wxMouseEvent& event);
    virtual void OnFunction(wxCommandEvent& event);
    virtual void OnScope(wxCommandEvent& event);
    virtual void OnScopeListMouseDown(wxMouseEvent& event);
};
#endif // NAVBARCONTROL_H
