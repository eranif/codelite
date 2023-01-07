#ifndef SPLITTERWINDOWPAGE_H
#define SPLITTERWINDOWPAGE_H

#include "panel_wrapper.h" // Base class: PanelWrapper

class SplitterWindowPage : public PanelWrapper
{
public:
    SplitterWindowPage();
    virtual ~SplitterWindowPage();
    virtual wxcWidget* Clone() const;
    virtual wxString CppCtorCode() const;
};

#endif // SPLITTERWINDOWPAGE_H
