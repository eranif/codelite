#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H
#include "wxcrafter.h"

extern const wxEventType wxEVT_CMD_TEXT_EDITOR;

class TextEditor : public TextEditorBaseClass
{
public:
    TextEditor(wxWindow* parent);
    ~TextEditor() override = default;
    void OnKeyDown(wxKeyEvent& event) override;
    void OnKillFocus(wxFocusEvent& event) override;
    void OnTextEnter(wxCommandEvent& event) override;

    void SetValue(const wxString& value);
    void MoveTo(const wxRect& rect);
};
#endif // TEXTEDITOR_H
