#ifndef EDITOROPTIONSGENERALGUIDESPANEL_H
#define EDITOROPTIONSGENERALGUIDESPANEL_H

#include "editoroptionsgeneralguidespanelbase.h"
#include "optionsconfig.h"
#include "treebooknodebase.h"

class EditorOptionsGeneralGuidesPanel : public EditorOptionsGeneralGuidesPanelBase,
                                        public TreeBookNode<EditorOptionsGeneralGuidesPanel>
{
public:
    EditorOptionsGeneralGuidesPanel(wxWindow* parent);
    virtual ~EditorOptionsGeneralGuidesPanel();
    void Save(OptionsConfigPtr options) override;

protected:
    virtual void OnUseRelativeLineNumbersUI(wxUpdateUIEvent& event);
    void OnDisplayLineNumbersUI(wxUpdateUIEvent& event) override;
    void OnHighlightCaretLineUI(wxUpdateUIEvent& event) override;
    void OnHighlightDebuggerLineUI(wxUpdateUIEvent& event) override;
};
#endif // EDITOROPTIONSGENERALGUIDESPANEL_H
