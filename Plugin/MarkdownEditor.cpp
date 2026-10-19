#include "MarkdownEditor.hpp"

#include "aui/clAuiToolBarArt.h"
#include "bitmap_loader.h"
#include "cl_config.h"
#include "codelite_events.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "imanager.h"

#include <algorithm>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/stc/stc.h>
#include <wx/uri.h>
#include <wx/utils.h>
#include <wx/xrc/xmlres.h>

namespace
{
// The buttons of the toolbar (the names are turned into IDs with XRCID)
constexpr const char* kToolView = "markdown_editor_view";
constexpr const char* kToolEdit = "markdown_editor_edit";
constexpr const char* kToolContents = "markdown_editor_contents";

/// The mode that the user chose with the buttons: true is the view (the document), false is the editor
const wxString kConfigViewMode = "MarkdownEditor/ViewMode";

/// Is the table of contents shown in the view?
const wxString kConfigShowContents = "MarkdownEditor/ShowContents";
} // namespace

MarkdownEditor::MarkdownEditor(wxWindow* parent, const EditorFactory& createEditor)
    : clEditorHost(parent, wxID_ANY)
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(sizer);

    m_toolbar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE);
    sizer->Add(m_toolbar, wxSizerFlags().Expand());

    m_book = new wxSimplebook(this, wxID_ANY);
    sizer->Add(m_book, wxSizerFlags(1).Expand());

    // Page 0: the rendered document, with its table of contents
    m_view = new MarkdownViewCtrl(m_book);
    m_text = m_view->GetTextCtrl();
    m_view->ShowContents(clConfig::Get().Read(kConfigShowContents, false));
    m_book->AddPage(m_view, _("View"));

    // Page 1: the editor. The caller creates it: this class does not know the editor class of the application
    m_hosted = createEditor(m_book);
    m_editor = m_hosted->GetCtrl();
    m_book->AddPage(m_editor, _("Edit"));

    m_toolbar->Bind(wxEVT_MENU, &MarkdownEditor::OnShowView, this, XRCID(kToolView));
    m_toolbar->Bind(wxEVT_MENU, &MarkdownEditor::OnShowEdit, this, XRCID(kToolEdit));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &MarkdownEditor::OnModeUI, this, XRCID(kToolView));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &MarkdownEditor::OnModeUI, this, XRCID(kToolEdit));
    m_toolbar->Bind(wxEVT_MENU, &MarkdownEditor::OnToggleContents, this, XRCID(kToolContents));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &MarkdownEditor::OnContentsUI, this, XRCID(kToolContents));
    m_text->Bind(wxEVT_TEXT_URL, &MarkdownEditor::OnLinkClicked, this);
    // Any change of the text (typing, loading the file, reloading it from the disk) makes the view outdated
    m_editor->Bind(wxEVT_STC_MODIFIED, &MarkdownEditor::OnEditorChanged, this);
    m_editor->Bind(wxEVT_STC_ZOOM, &MarkdownEditor::OnEditorZoomed, this);
    EventNotifier::Get()->Bind(wxEVT_BITMAPS_UPDATED, &MarkdownEditor::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &MarkdownEditor::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &MarkdownEditor::OnThemeChanged, this);

    BuildToolbar();
    m_text->Bind(wxEVT_CONTEXT_MENU, &MarkdownEditor::OnContextMenu, this);

    // The keyboard: the control has its own accelerators (Ctrl-C, Ctrl-A, ...). They send the menu event to the control
    // itself, and the event never gets to the application. Our handlers are bound to the control, they run before its
    // own handlers
    m_text->Bind(wxEVT_MENU, &MarkdownEditor::OnCopy, this, wxID_COPY);
    m_text->Bind(wxEVT_MENU, &MarkdownEditor::OnSelectAll, this, wxID_SELECTALL);
    // The menu bar (Edit > Copy) and the context menu: the event goes to the frame and then to the application
    wxTheApp->Bind(wxEVT_MENU, &MarkdownEditor::OnCopy, this, wxID_COPY);
    wxTheApp->Bind(wxEVT_MENU, &MarkdownEditor::OnSelectAll, this, wxID_SELECTALL);
    wxTheApp->Bind(wxEVT_UPDATE_UI, &MarkdownEditor::OnCopyUI, this, wxID_COPY);

    // Start in the mode that the user chose the last time (the document, by default). The file is loaded by the caller
    // after the constructor, so render when it is done (in the edit mode the document is rendered when the user
    // switches to it)
    m_mode = clConfig::Get().Read(kConfigViewMode, true) ? Mode::View : Mode::Edit;
    m_book->SetSelection(m_mode == Mode::View ? 0 : 1);
    ScheduleRender();
}

MarkdownEditor::~MarkdownEditor()
{
    wxTheApp->Unbind(wxEVT_MENU, &MarkdownEditor::OnCopy, this, wxID_COPY);
    wxTheApp->Unbind(wxEVT_MENU, &MarkdownEditor::OnSelectAll, this, wxID_SELECTALL);
    wxTheApp->Unbind(wxEVT_UPDATE_UI, &MarkdownEditor::OnCopyUI, this, wxID_COPY);

    EventNotifier::Get()->Unbind(wxEVT_BITMAPS_UPDATED, &MarkdownEditor::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &MarkdownEditor::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &MarkdownEditor::OnThemeChanged, this);
}

void MarkdownEditor::BuildToolbar()
{
    auto* icons = clGetManager()->GetStdIcons();
    m_toolbar->Clear();
    clAuiToolBarArt::AddTool(m_toolbar,
                             XRCID(kToolView),
                             _("View"),
                             icons->LoadBitmap("markdown-view"),
                             _("Show the document"),
                             wxITEM_CHECK);
    clAuiToolBarArt::AddTool(m_toolbar,
                             XRCID(kToolEdit),
                             _("Edit"),
                             icons->LoadBitmap("markdown-edit"),
                             _("Edit the text of the file"),
                             wxITEM_CHECK);
    m_toolbar->AddSeparator();
    clAuiToolBarArt::AddTool(m_toolbar,
                             XRCID(kToolContents),
                             _("Contents"),
                             icons->LoadBitmap("markdown-toc"),
                             _("Show or hide the table of contents"),
                             wxITEM_CHECK);
    clAuiToolBarArt::Finalise(m_toolbar);
    m_toolbar->Realize();
    // The new tools can have another size (a new theme has other icons): tell the sizer, or the toolbar keeps its old
    // size until the window is resized
    m_toolbar->InvalidateBestSize();
    Layout();
    m_toolbar->Refresh();
}

void MarkdownEditor::SetMode(Mode mode)
{
    m_mode = mode;
    if (mode == Mode::View) {
        if (m_viewOutdated) {
            RenderView();
        }
        m_book->SetSelection(0);
    } else {
        m_book->SetSelection(1);
        // The editor was hidden: the first visible line and the caret that were set while it had no size (when the
        // session is restored) can be off the screen. Scintilla can scroll to the caret only when the editor is shown
        m_editor->CallAfter([editor = m_editor]() { editor->EnsureCaretVisible(); });
    }
    FocusContent();
}

void MarkdownEditor::FocusContent()
{
    if (m_mode == Mode::View) {
        m_text->SetFocus();
    } else {
        m_editor->SetFocus();
        m_editor->SetSTCFocus(true);
    }
}

clRichTextMarkdownRenderer::Theme MarkdownEditor::MakeViewTheme() const
{
    // The colours and the code font of the editor. The text of the document uses the font of the user interface, with
    // the size of the font of the editor. The zoom of the editor (Ctrl + mouse wheel) is in points, and it is not part
    // of the font of the style
    wxFont editor_font = m_editor->StyleGetFont(wxSTC_STYLE_DEFAULT);
    editor_font.SetFractionalPointSize(std::max(1.0, editor_font.GetFractionalPointSize() + m_editor->GetZoom()));
    wxFont text_font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    text_font.SetFractionalPointSize(editor_font.GetFractionalPointSize());
    return clRichTextMarkdownRenderer::MakeTheme(text_font,
                                                 editor_font,
                                                 m_editor->StyleGetForeground(wxSTC_STYLE_DEFAULT),
                                                 m_editor->StyleGetBackground(wxSTC_STYLE_DEFAULT));
}

void MarkdownEditor::RenderView()
{
    m_viewOutdated = false;
    wxBusyCursor bc{};
    clGetManager()->SetStatusMessage(_("Rendering Markdown..."));
    wxSafeYield();
    m_view->Render(m_editor->GetText(), MakeViewTheme(), m_hosted->GetFileName().GetPath());
    clGetManager()->SetStatusMessage(wxEmptyString);
}

void MarkdownEditor::ScheduleRender()
{
    m_viewOutdated = true;
    if (m_renderScheduled || m_mode != Mode::View) {
        // In the edit mode the view is rendered when the user switches to it
        return;
    }
    m_renderScheduled = true;
    CallAfter([this]() {
        m_renderScheduled = false;
        if (m_mode == Mode::View && m_viewOutdated) {
            RenderView();
        }
    });
}

void MarkdownEditor::OpenLink(const wxString& url)
{
    // The scheme is not case sensitive
    const wxString lower_url = url.Lower();
    if (lower_url.StartsWith("http://") || lower_url.StartsWith("https://") || lower_url.StartsWith("mailto:")) {
        ::wxLaunchDefaultBrowser(url);
        return;
    }

    // A link to a heading of this document ("#some-heading"): scroll to it. It is not a path
    if (url.StartsWith("#")) {
        if (!m_view->GoToAnchor(url.Mid(1))) {
            clDEBUG() << "Markdown: no such heading:" << url << endl;
        }
        return;
    }

    // A path. The text of the link is tried as it is first (a file name can have a '#' or a '?'), and then without
    // the fragment and the query. (A link inside the document, "#section", is not supported yet)
    wxArrayString candidates;
    candidates.Add(url);
    const wxString without_fragment = url.BeforeFirst('#').BeforeFirst('?');
    if (!without_fragment.empty() && without_fragment != url) {
        candidates.Add(without_fragment);
    }

    for (const wxString& candidate : candidates) {
        // The document can come from an untrusted source: an address that is not a local path (another scheme, a
        // network path) never reaches the file system
        const wxString path = clRichTextMarkdownRenderer::LocalPathFromUrl(candidate);
        if (path.empty()) {
            continue;
        }

        wxFileName fn(path);
        if (fn.IsRelative()) {
            fn.MakeAbsolute(m_hosted->GetFileName().GetPath());
        }
        // The static function: the member function DirExists() asks about the directory part of the path (the folder
        // that holds the file), it is true for every path in an existing folder
        if (wxFileName::DirExists(fn.GetFullPath())) {
            FileUtils::OpenFileExplorer(fn.GetFullPath());
            return;
        }
        if (fn.FileExists()) {
            // Opening a Markdown file opens its page too
            clGetManager()->OpenFile(fn.GetFullPath());
            return;
        }
    }
    clDEBUG() << "Markdown: can not open the link:" << url << endl;
}

void MarkdownEditor::OnShowView(wxCommandEvent& event)
{
    wxUnusedVar(event);
    SetMode(Mode::View);
    // The user chose it: the next Markdown file opens in this mode too. (A command that shows the editor, like Find or
    // go to line, does not change the choice)
    clConfig::Get().Write(kConfigViewMode, true);
}

void MarkdownEditor::OnShowEdit(wxCommandEvent& event)
{
    wxUnusedVar(event);
    SetMode(Mode::Edit);
    clConfig::Get().Write(kConfigViewMode, false);
}

void MarkdownEditor::OnToggleContents(wxCommandEvent& event)
{
    wxUnusedVar(event);
    // The user chose it: the next Markdown file shows the table of contents in the same way
    const bool show = !m_view->IsContentsShown();
    m_view->ShowContents(show);
    clConfig::Get().Write(kConfigShowContents, show);
}

void MarkdownEditor::OnContentsUI(wxUpdateUIEvent& event)
{
    // The table of contents is part of the view
    event.Enable(m_mode == Mode::View);
    event.Check(m_view->IsContentsShown());
}

void MarkdownEditor::OnEditorZoomed(wxStyledTextEvent& event)
{
    event.Skip();
    // The size of the document follows the size of the text of the editor
    ScheduleRender();
}

void MarkdownEditor::OnModeUI(wxUpdateUIEvent& event)
{
    // The check mark of the button of the mode that is shown
    const bool is_view_button = event.GetId() == XRCID(kToolView);
    event.Check(is_view_button == (m_mode == Mode::View));
}

void MarkdownEditor::OnLinkClicked(wxTextUrlEvent& event)
{
    // Not skipped: we handled the link
    OpenLink(event.GetString());
}

void MarkdownEditor::OnEditorChanged(wxStyledTextEvent& event)
{
    event.Skip();
    if (event.GetModificationType() & (wxSTC_MOD_INSERTTEXT | wxSTC_MOD_DELETETEXT)) {
        ScheduleRender();
    }
}

void MarkdownEditor::OnThemeChanged(clCommandEvent& event)
{
    event.Skip();
    // The icons of the toolbar have the colours of the theme, and so has the view
    BuildToolbar();
    ScheduleRender();
}

void MarkdownEditor::OnContextMenu(wxContextMenuEvent& event)
{
    wxUnusedVar(event);
    wxMenu menu;
    menu.Append(wxID_COPY);
    menu.Append(wxID_SELECTALL);
    PopupMenu(&menu);
}

void MarkdownEditor::OnCopy(wxCommandEvent& event)
{
    if (m_text == nullptr || m_text != wxWindow::FindFocus()) {
        event.Skip();
        return;
    }

    if (m_text->CanCopy()) {
        m_text->Copy();
    }
}

void MarkdownEditor::OnCopyUI(wxUpdateUIEvent& event)
{
    if (m_text == nullptr || m_text != wxWindow::FindFocus()) {
        event.Skip();
        return;
    }
    event.Enable(m_text->CanCopy());
}

void MarkdownEditor::OnSelectAll(wxCommandEvent& event)
{
    if (m_text == nullptr || m_text != wxWindow::FindFocus()) {
        event.Skip();
        return;
    }
    m_text->SelectAll();
}
