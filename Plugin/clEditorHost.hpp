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
    /// The only constructor, so every host is created in the same way.
    ///
    /// The host is hidden when it is created. A new window is shown at once, but the page is added to the book later,
    /// and the book shows its pages when they are selected: without this the page would flash at the top left corner of
    /// the book. Hiding the host is enough for its children (a child is not visible if its parent is hidden), so they
    /// do not need to hide themselves.
    explicit clEditorHost(wxWindow* parent, wxWindowID id = wxID_ANY)
        : wxWindow(parent, id)
    {
        Hide();
    }
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
