#ifndef EDITOROPTIONSGENERALGUIDESPANEL_H
#define EDITOROPTIONSGENERALGUIDESPANEL_H

#include "OptionsConfigPage.hpp"

class EditorOptionsGeneralGuidesPanel : public OptionsConfigPage
{
public:
    EditorOptionsGeneralGuidesPanel(wxWindow* parent, OptionsConfigPtr options);
    virtual ~EditorOptionsGeneralGuidesPanel();
};
#endif // EDITOROPTIONSGENERALGUIDESPANEL_H
