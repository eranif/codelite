#pragma once

#include "OptionsConfigPage.hpp"

class EditorOptionsGeneralGuidesPanel : public OptionsConfigPage
{
public:
    EditorOptionsGeneralGuidesPanel(wxWindow* parent, OptionsConfigPtr options);
    ~EditorOptionsGeneralGuidesPanel() override = default;
};
