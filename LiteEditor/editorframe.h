#ifndef EDITORFRAME_H
#define EDITORFRAME_H

#include "wxcrafter.h"
#include <list>
#include "cl_command_event.h"

wxDECLARE_EVENT(wxEVT_DETACHED_EDITOR_CLOSED, clCommandEvent);

class LEditor;
class EditorFrame : public EditorFrameBase
{
public:
    typedef std::list<EditorFrame*> List_t;

protected:
    virtual void OnClose(wxCommandEvent& event);
    virtual void OnCloseUI(wxUpdateUIEvent& event);
    virtual void OnReload(wxCommandEvent& event);
    virtual void OnReloadUI(wxUpdateUIEvent& event);
    virtual void OnSave(wxCommandEvent& event);
    virtual void OnSaveUI(wxUpdateUIEvent& event);
    LEditor *m_editor;

public:
    EditorFrame(wxWindow* parent, LEditor* editor);
    virtual ~EditorFrame();
    LEditor* GetEditor() {
        return m_editor;
    }
};
#endif // EDITORFRAME_H
