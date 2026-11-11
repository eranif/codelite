#pragma once

#include "controls/Containers/panel_wrapper.h" // Base class: PanelWrapper

class SplitterWindowPage : public PanelWrapper
{
public:
    SplitterWindowPage();
    ~SplitterWindowPage() override = default;
    wxcWidget* Clone() const override;
    wxString CppCtorCode() const override;
};
