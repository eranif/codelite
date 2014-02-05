#ifndef EDITORFRAME_H
#define EDITORFRAME_H

#include "wxcrafter.h"
#include <list>
#include "cl_command_event.h"

class MyMenuBar;
wxDECLARE_EVENT(wxEVT_DETACHED_EDITOR_CLOSED, clCommandEvent);

class LEditor;
class QuickFindBar;
class EditorFrame : public EditorFrameBase
{
public:
    typedef std::list<EditorFrame*> List_t;

protected:
    LEditor *m_editor;
    QuickFindBar* m_findBar;
    MyMenuBar *m_myMenuBar;
    
protected:
    virtual void OnFindUI(wxUpdateUIEvent& event);
    virtual void OnFind(wxCommandEvent& event);
    virtual void OnClose(wxCommandEvent& event);
    virtual void OnCloseUI(wxUpdateUIEvent& event);

public:
    EditorFrame(wxWindow* parent, LEditor* editor);
    virtual ~EditorFrame();
    LEditor* GetEditor() {
        return m_editor;
    }
};
#endif // EDITORFRAME_H
