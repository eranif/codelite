#pragma once

#include "clMarkdownParser.hpp"
#include "codelite_exports.h"

#include <vector>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/richtext/richtextctrl.h>
#include <wx/string.h>

/// A wxRichTextCtrl for a rendered document. It copies the selected text as plain text with `\n` for a line break (the
/// control keeps a line break inside a paragraph as a special character, wxRichTextLineBreakChar, and the standard copy
/// gives that character: it is not a new line when it is pasted somewhere else).
class WXDLLIMPEXP_SDK clMarkdownTextCtrl : public wxRichTextCtrl
{
public:
    using wxRichTextCtrl::wxRichTextCtrl;

    void Copy() override;
};

/// Show a Markdown document in a wxRichTextCtrl (use a clMarkdownTextCtrl to copy text with the right line breaks).
///
/// It is a clMarkdownParser that builds the content of the control while the document is parsed.
///
///     auto* ctrl = new wxRichTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
///                                     wxRE_READONLY | wxBORDER_NONE);
///     clRichTextMarkdownRenderer renderer(ctrl);
///     renderer.SetBaseDir(file.GetPath());      // to find the images that have a relative path
///     renderer.Render(markdown_text);
///     // Links: handle wxEVT_TEXT_URL of the control. event.GetString() is the address of the link
///     ctrl->Bind(wxEVT_TEXT_URL, [](wxTextUrlEvent& event) { ... event.GetString() ... });
///
/// What is supported: headings, paragraphs, emphasis, strong, underline, strikethrough, inline code and code blocks
/// (monospace font and a background, no syntax colours), quotes, ordered, unordered and task lists, links, images that
/// are files (not the ones that need the network), tables and horizontal rules. Raw HTML is shown as text.
///
/// The control must not be modified by something else while `Render()` runs. `Render()` replaces the whole content, and
/// it does not keep the scroll position.
class WXDLLIMPEXP_SDK clRichTextMarkdownRenderer : public clMarkdownParser
{
public:
    /// Fonts and colours
    struct Theme {
        wxFont font;          ///< The font of the text
        wxFont monospaceFont; ///< The font of the code
        wxColour text;
        wxColour background;
        wxColour link;
        wxColour muted;          ///< Raw HTML and the images that could not be loaded
        wxColour heading;        ///< The text of the headings
        wxColour bullet;         ///< The bullets and the numbers of the lists
        wxColour inlineCode;     ///< The text of an inline code (`code`). A code block uses `text`
        wxColour codeBackground; ///< Code, and the header of a table
        wxColour border;         ///< Tables, code blocks and horizontal rules
        wxColour quote;          ///< The bar at the left of a quote
    };

    /// \param ctrl the control to fill. It is not owned
    explicit clRichTextMarkdownRenderer(wxRichTextCtrl* ctrl);
    ~clRichTextMarkdownRenderer() override = default;

    /// A theme from the system colours and from the font of `win`. The code background and the other colours are made
    /// from the background colour, for a light or a dark background
    static Theme MakeDefaultTheme(const wxWindow* win);

    /// A theme from the given fonts and from the colour of the text and of the background (for example the ones of an
    /// editor). The headings, the bullets and the borders have colours of the C++ lexer of the current theme (a class,
    /// a number and the default style). The other colours (link, code background, ...) are made from the background,
    /// for a light or a dark background
    static Theme MakeTheme(const wxFont& font,
                           const wxFont& monospaceFont,
                           const wxColour& text,
                           const wxColour& background);

    /// The path of the local file that the address `url` (the address of a link or of an image) points to, or an empty
    /// string if it does not point to a local file. A document can come from an untrusted source (a README of a cloned
    /// repository, the answer of an assistant), so an address that would make us connect to another computer is never
    /// accepted. It is an allow-list:
    /// - A scheme other than `file` (`http:`, `ftp:`, `data:`, ...) is rejected, `file://host/...` too.
    /// - A network path is rejected (`//server/share`, `\\server\share`, in any mix of '/' and '\', also when it is
    ///   percent-encoded or after the `file:` prefix).
    /// Use it before the file system is touched with an address of a document. The result can be relative.
    static wxString LocalPathFromUrl(const wxString& url);

    void SetTheme(const Theme& theme) { m_theme = theme; }
    const Theme& GetTheme() const { return m_theme; }

    /// The folder to which the paths of the images are relative
    void SetBaseDir(const wxString& dir) { m_baseDir = dir; }

    /// The space between the document and the border of the control: `horizontal` at the left and at the right,
    /// `vertical` at the top and at the bottom. In DIP. Default: 24 and 12
    void SetMargins(int horizontal, int vertical)
    {
        m_marginHorizontal = horizontal;
        m_marginVertical = vertical;
    }

    /// Replace the content of the control with `markdown`. Call it from the GUI thread.
    /// \return false if the document could not be parsed
    bool Render(const wxString& markdown);

protected:
    void OnEnterBlock(Block block, const BlockInfo& info) override;
    void OnLeaveBlock(Block block, const BlockInfo& info) override;
    void OnEnterSpan(Span span, const SpanInfo& info) override;
    void OnLeaveSpan(Span span, const SpanInfo& info) override;
    void OnText(TextType type, const wxString& text) override;

private:
    struct ListState {
        bool ordered = false;
        bool tight = false;
        unsigned nextNumber = 1; ///< The number of the next item of an ordered list
    };

    void ResetState();
    /// The attributes of the characters (the font, the colour and the link) for the state of the parser
    wxRichTextAttr BuildCharAttr() const;
    /// The attributes of the paragraph (the indent, the bullet, the spacing, ...) for the state of the parser
    wxRichTextAttr BuildParagraphAttr(bool firstInListItem) const;
    /// Add a new, empty paragraph at the end of the current container (the document or a table cell)
    wxRichTextParagraph* AddParagraph(wxRichTextAttr& attr);
    /// The paragraph we are writing to. It is created when the first content of a block arrives
    wxRichTextParagraph* EnsureParagraph();
    void AppendText(const wxString& text, const wxRichTextAttr& attr);
    /// Text that can have new lines (code, raw HTML). The new lines are line breaks of the paragraph, and the last new
    /// line of a block is dropped
    void AppendMultilineText(const wxString& text, const wxRichTextAttr& attr);
    void AppendImage(const wxString& url, const wxString& alt);
    wxImage LoadLocalImage(const wxString& url) const;
    void EnterTableCell(bool header, Align align);
    void LeaveTableCell();

    wxRichTextCtrl* m_ctrl = nullptr;
    Theme m_theme;
    wxString m_baseDir;
    int m_marginHorizontal = 24;
    int m_marginVertical = 12;

    // Where we write: the document, or a table cell
    wxRichTextParagraphLayoutBox* m_container = nullptr;
    wxRichTextParagraph* m_para = nullptr;
    /// While a code block is written: the container of the block, the code is in a box inside it
    wxRichTextParagraphLayoutBox* m_codeOwner = nullptr;

    // The state of the parser
    std::vector<ListState> m_lists;
    int m_quoteDepth = 0;
    unsigned m_headingLevel = 0;
    bool m_inCodeBlock = false;
    bool m_inHtmlBlock = false;
    int m_pendingBreaks = 0; ///< New lines of a code or HTML block that are written only if more text follows
    int m_emphasis = 0;
    int m_strong = 0;
    int m_underline = 0;
    int m_strikethrough = 0;
    int m_code = 0;
    std::vector<wxString> m_links;

    // A list item
    bool m_itemPending = false; ///< The first paragraph of the item is not created yet
    bool m_itemTask = false;
    bool m_itemTaskChecked = false;

    // An image: the alternative text arrives as text, before we know if the image can be loaded
    int m_imageDepth = 0;
    wxString m_imageUrl;
    wxString m_imageAlt;

    // A table
    wxRichTextTable* m_table = nullptr;
    wxRichTextParagraphLayoutBox* m_tableOwner = nullptr; ///< The container of the table
    wxRichTextCell* m_cell = nullptr;
    unsigned m_tableColumns = 0;
    int m_tableRows = 0;
    int m_tableColumn = -1;
    bool m_inTableHeader = false;
    Align m_cellAlign = Align::Default;
};
