#ifndef EDITOROPTIONSGENERALEDIT_H
#define EDITOROPTIONSGENERALEDIT_H

#include "editoroptionsgeneralguidespanelbase.h"
#include "treebooknodebase.h"
#include "optionsconfig.h"

class EditorOptionsGeneralEdit : public EditorOptionsGeneralEditBase,
                                 public TreeBookNode<EditorOptionsGeneralEdit>
{
public:
    EditorOptionsGeneralEdit(wxWindow* parent);
    virtual ~EditorOptionsGeneralEdit();
    void Save(OptionsConfigPtr options);
protected:
    virtual void OnValueChanged(wxPropertyGridEvent& event);
};
#endif // EDITOROPTIONSGENERALEDIT_H
