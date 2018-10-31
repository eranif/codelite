#include "NodeJSDebuggerBase.h"
#include "bookmark_manager.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include <algorithm>
#include <wx/stc/stc.h>

NodeJSDebuggerBase::NodeJSDebuggerBase() {}

NodeJSDebuggerBase::~NodeJSDebuggerBase() {}

void NodeJSDebuggerBase::SetDebuggerMarker(IEditor* editor, int lineno)
{
    wxStyledTextCtrl* stc = editor->GetCtrl();
    stc->MarkerDeleteAll(smt_indicator);
    stc->MarkerAdd(lineno, smt_indicator);
    int caretPos = stc->PositionFromLine(lineno);
    stc->SetSelection(caretPos, caretPos);
    stc->SetCurrentPos(caretPos);
    stc->EnsureCaretVisible();
    editor->CenterLine(lineno);
#ifdef __WXOSX__
    stc->Refresh();
#endif
}

void NodeJSDebuggerBase::ClearDebuggerMarker()
{
    IEditor::List_t editors;
    clGetManager()->GetAllEditors(editors);
    std::for_each(editors.begin(), editors.end(),
                  [&](IEditor* editor) { editor->GetCtrl()->MarkerDeleteAll(smt_indicator); });
}

void NodeJSDebuggerBase::DoHighlightLine(const wxString& filename, int lineNo)
{
    IEditor* activeEditor = clGetManager()->OpenFile(filename, "", lineNo - 1);
    if(activeEditor) { SetDebuggerMarker(activeEditor, lineNo - 1); }
}

void NodeJSDebuggerBase::SetDebuggerMarker(const wxString& path, int lineno) { DoHighlightLine(path, lineno); }
