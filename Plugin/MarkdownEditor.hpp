#pragma once

#include "MarkdownViewCtrl.hpp"
#include "clEditorHost.hpp"
#include "clRichTextMarkdownRenderer.hpp"
#include "cl_command_event.h"
#include "codelite_exports.h"

#include <functional>
#include <wx/aui/auibar.h>
#include <wx/simplebook.h>
#include <wx/stc/stc.h>
#include <wx/window.h>

/// The page of a Markdown file in the main book. It shows the file in 2 ways, and a toolbar switches between them:
///
///     [toolbar: View | Edit]
///     [simplebook]
///       [0] MarkdownViewCtrl  (View: the rendered document and its table of contents)
///       [1] editor            (Edit: the text of the file)
///
/// The editor is a normal editor of the file: Save, the modified mark, closing, the session and the plugins work with
/// it as with any other editor (see clEditorHost). The view is made from the text that is in the editor, so it also
/// shows the changes that are not saved yet.
class WXDLLIMPEXP_SDK MarkdownEditor : public clEditorHost
{
public:
    enum class Mode {
        View, ///< The rendered document
        Edit, ///< The editor
    };

    /// Creates the editor of the page. The parent is the window that it must be created with. The editor must be a
    /// wxStyledTextCtrl (`IEditor::GetCtrl()` returns it)
    using EditorFactory = std::function<IEditor*(wxWindow* parent)>;

    /// The editor is created here (with `createEditor`), but the caller has to load a file in it
    MarkdownEditor(wxWindow* parent, const EditorFactory& createEditor);
    ~MarkdownEditor() override;

    Mode GetMode() const { return m_mode; }
    void SetMode(Mode mode);

    // clEditorHost
    IEditor* GetHostedEditor() const override { return m_hosted; }
    /// Does nothing if the editor is already shown (it is called for every text command)
    void ShowHostedEditor() override
    {
        if (m_mode != Mode::Edit) {
            SetMode(Mode::Edit);
        }
    }
    bool IsHostedEditorShown() const override { return m_mode == Mode::Edit; }
    void FocusContent() override;

    // wxWindow
    void SetFocus() override { FocusContent(); }

private:
    /// (Re)create the buttons. Called again when the theme changes, to load the icons of the new theme
    void BuildToolbar();
    /// Fill the view from the text of the editor
    void RenderView();
    /// Render the view later, when the event that is being processed is done (many changes make one render)
    void ScheduleRender();
    /// The fonts and colours of the editor, for the view
    clRichTextMarkdownRenderer::Theme MakeViewTheme() const;
    /// Open the address of a link: a web address, or a path (relative to the folder of the file)
    void OpenLink(const wxString& url);

    void OnShowView(wxCommandEvent& event);
    void OnShowEdit(wxCommandEvent& event);
    void OnModeUI(wxUpdateUIEvent& event);
    void OnToggleContents(wxCommandEvent& event);
    void OnContentsUI(wxUpdateUIEvent& event);
    void OnLinkClicked(wxTextUrlEvent& event);
    void OnEditorChanged(wxStyledTextEvent& event);
    void OnEditorZoomed(wxStyledTextEvent& event);
    void OnThemeChanged(clCommandEvent& event);
    void OnContextMenu(wxContextMenuEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnCopyUI(wxUpdateUIEvent& event);
    void OnSelectAll(wxCommandEvent& event);

    wxAuiToolBar* m_toolbar = nullptr;
    wxSimplebook* m_book = nullptr;
    MarkdownViewCtrl* m_view = nullptr;
    clMarkdownTextCtrl* m_text = nullptr; ///< The text of the document, inside the view
    IEditor* m_hosted = nullptr;          ///< The editor, as an IEditor
    wxStyledTextCtrl* m_editor = nullptr; ///< The same editor, as a control
    Mode m_mode = Mode::View;
    /// The text or the theme changed after the last render
    bool m_viewOutdated = true;
    bool m_renderScheduled = false;
};
