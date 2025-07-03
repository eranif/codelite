#ifndef SPLITTERWINDOWPAGE_H
#define SPLITTERWINDOWPAGE_H

#include "panel_wrapper.h" // Base class: PanelWrapper

class SplitterWindowPage : public PanelWrapper
{
public:
    SplitterWindowPage();
    ~SplitterWindowPage() override = default;
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
};

#endif // SPLITTERWINDOWPAGE_H
