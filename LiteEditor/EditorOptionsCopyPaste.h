#ifndef EDITOROPTIONSCOPYPASTE_H
#define EDITOROPTIONSCOPYPASTE_H

#include "editor_options_copy_paste.h"
#include "treebooknodebase.h"
#include "optionsconfig.h"

class EditorOptionsCopyPaste : public EditorOptionsCopyPasteBase, public TreeBookNode<EditorOptionsCopyPaste>
{
public:
    EditorOptionsCopyPaste(wxWindow* parent);
    virtual ~EditorOptionsCopyPaste();
    virtual void Save(OptionsConfigPtr options);
};
#endif // EDITOROPTIONSCOPYPASTE_H
