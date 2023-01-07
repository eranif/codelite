#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H
#include "wxcrafter.h"

extern const wxEventType wxEVT_CMD_TEXT_EDITOR;

class TextEditor : public TextEditorBaseClass
{
public:
    TextEditor(wxWindow* parent);
    virtual ~TextEditor();
    virtual void OnKeyDown(wxKeyEvent& event);
    virtual void OnKillFocus(wxFocusEvent& event);
    virtual void OnTextEnter(wxCommandEvent& event);

    void SetValue(const wxString& value);
    void MoveTo(const wxRect& rect);
};
#endif // TEXTEDITOR_H
