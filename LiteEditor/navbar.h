#ifndef NAVBAR_H
#define NAVBAR_H
#include "wxcrafter.h"

class NavBar : public NavBarControlBaseClass
{
public:
    NavBar(wxWindow* parent);
    virtual ~NavBar();
protected:
    virtual void OnFunction(wxCommandEvent& event);
    virtual void OnScope(wxCommandEvent& event);
};
#endif // NAVBAR_H
