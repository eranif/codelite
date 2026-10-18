#pragma once

#include "clRichTextMarkdownRenderer.hpp"
#include "codelite_exports.h"

#include <memory>
#include <wx/dataview.h>
#include <wx/splitter.h>

/// A rendered Markdown document with its table of contents:
///
///     [wxSplitterWindow]
///       LEFT:  clMarkdownTextCtrl  (the document, read only)
///       RIGHT: wxDataViewListCtrl  (the table of contents: one row for each heading)
///
/// A click (or Enter) on a row of the table of contents scrolls the document to the heading.
///
///     auto* view = new MarkdownViewCtrl(parent);
///     view->Render(markdown_text, theme, file.GetPath());
///     // Links: handle wxEVT_TEXT_URL of the text control
///     view->GetTextCtrl()->Bind(wxEVT_TEXT_URL, ...);
class WXDLLIMPEXP_SDK MarkdownViewCtrl : public wxSplitterWindow
{
public:
    explicit MarkdownViewCtrl(wxWindow* parent);
    ~MarkdownViewCtrl() override = default;

    /// The control that shows the document (the left side)
    clMarkdownTextCtrl* GetTextCtrl() const { return m_text; }

    /// The table of contents (the right side)
    wxDataViewListCtrl* GetContentsCtrl() const { return m_contents; }

    /// Replace the document and the table of contents. The scroll position is not kept
    /// \param markdown the text of the document
    /// \param theme the fonts and the colours of the document. The table of contents does not use them: it has the native look
    /// \param baseDir the folder to which the paths of the images are relative
    /// \return false if the document could not be parsed
    bool Render(const wxString& markdown, const clRichTextMarkdownRenderer::Theme& theme, const wxString& baseDir);

    /// Scroll the document to the heading that `anchor` names, see clRichTextMarkdownRenderer::GoToAnchor()
    /// \return false if there is no such heading
    bool GoToAnchor(const wxString& anchor);

    /// Show or hide the table of contents. The document uses the whole width while it is hidden
    void ShowContents(bool show);
    bool IsContentsShown() const { return IsSplit(); }

private:
    /// Fill the table of contents from the headings of the document that was rendered last
    void FillContents();
    void OnContentsSelected(wxDataViewEvent& event);
    void OnContentsSize(wxSizeEvent& event);
    void OnSize(wxSizeEvent& event);

    clMarkdownTextCtrl* m_text = nullptr;
    wxDataViewListCtrl* m_contents = nullptr;
    std::unique_ptr<clRichTextMarkdownRenderer> m_renderer;
    /// The table of contents gets its width when the splitter has a size for the first time
    bool m_sashInitialised = false;
};
