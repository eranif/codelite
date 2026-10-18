#pragma once

#include "codelite_exports.h"
#include "ieditor.h"

#include <wx/window.h>

/// A page of the main book that contains an editor inside it (and not the editor itself), for example the Markdown
/// page that can show the file as a document or as an editor.
///
/// The code of the main book finds the editors of the pages with clEditorFromPage(), and it finds the page of an
/// editor with MainBook::PageOf(), so the hosted editor works as any other editor: Save, Save All, the modified mark
/// of the tab, closing, the session, find by file name, and so on.
class WXDLLIMPEXP_SDK clEditorHost : public wxWindow
{
public:
    using wxWindow::wxWindow;
    ~clEditorHost() override = default;

    /// The editor inside the page
    virtual IEditor* GetHostedEditor() const = 0;

    /// Make sure that the editor is visible. Called when the user wants to go to a place of the file (a line, a
    /// position) or runs a text command, and the page is showing the file in another way.
    /// It must be cheap and it must do nothing if the editor is already visible: it is called for every text command
    virtual void ShowHostedEditor() = 0;

    /// Is the editor visible now?
    virtual bool IsHostedEditorShown() const = 0;

    /// Move the keyboard focus to the content that is visible now
    virtual void FocusContent() = 0;
};

/// The host of the editor of `page`, or nullptr if the page is not a host
inline clEditorHost* clEditorHostFromPage(wxWindow* page) { return dynamic_cast<clEditorHost*>(page); }
