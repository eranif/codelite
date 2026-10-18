#include "clRichTextMarkdownRenderer.hpp"

#include <algorithm>
#include <iterator>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/settings.h>
#include <wx/uri.h>

namespace
{
// Sizes of the rich text control are in tenths of a millimetre
constexpr int kBulletIndent = 100;   ///< Each level of a list moves the bullet by this
constexpr int kBulletSubIndent = 60; ///< The text of an item starts this far after its bullet
constexpr int kQuoteIndent = 80;
constexpr int kCodeIndent = 40;
constexpr int kParagraphSpacing = 40;
constexpr int kTightSpacing = 5;
constexpr int kHeadingSpacingBefore = 80;
constexpr int kHeadingSpacingAfter = 30;
constexpr int kMaxImageWidth = 800; ///< In pixels
constexpr int kCellPadding = 4;     ///< In pixels
constexpr double kHeadingScale[] = {1.9, 1.6, 1.35, 1.2, 1.1, 1.0};

bool IsDark(const wxColour& colour)
{
    return (colour.Red() * 299 + colour.Green() * 587 + colour.Blue() * 114) / 1000 < 128;
}

/// The path of the local file that `url` points to, or an empty string if `url` is not a local file. The document can
/// come from an untrusted source (a README of a cloned repository, the answer of an assistant), so an address that
/// would make us connect to another computer must never be accepted:
/// - Only the `file` scheme is allowed (an allow-list: `http:`, `ftp:`, `data:`, ... are rejected).
/// - A network path (`//server/share`, `\\server\share`) is rejected, also after the `file:` prefix is removed.
wxString LocalImagePath(const wxString& url)
{
    wxString path = url;

    // A scheme is 2 or more characters followed by ':'. One character is a drive letter ("C:\dir")
    static const wxRegEx kScheme("^[A-Za-z][A-Za-z0-9+.-]+:", wxRE_EXTENDED);
    if (kScheme.Matches(path)) {
        if (!path.StartsWith("file://")) {
            return wxEmptyString;
        }
        // file:///C:/dir (or file:///home/user): the host is empty. file://host/dir has a host: not a local file
        path = path.Mid(7);
        if (path.empty() || path[0] != '/') {
            return wxEmptyString;
        }
        // /C:/dir -> C:/dir
        if (path.length() > 2 && path[2] == ':') {
            path = path.Mid(1);
        }
    }

    // Unescape first: "%5C%5C" is "\\"
    path = wxURI::Unescape(path);

    // Windows accepts '/' and '\' in any mix as separators, so "/\server\share", "\/server/share" and "//server/share"
    // are all network paths (also with more than 2 separators). Check a copy that has only '/'
    wxString normalized = path;
    normalized.Replace("\\", "/");
    if (normalized.StartsWith("//")) {
        return wxEmptyString;
    }
    return path;
}

/// Set the style, the colour and the width of the 4 borders
void SetBorders(wxRichTextAttr& attr, const wxColour& colour, int width_in_pixels)
{
    auto& borders = attr.GetTextBoxAttr().GetBorder();
    borders.SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
    borders.SetColour(colour);
    borders.SetWidth(width_in_pixels, wxTEXT_ATTR_UNITS_PIXELS);
}
} // namespace

clRichTextMarkdownRenderer::clRichTextMarkdownRenderer(wxRichTextCtrl* ctrl)
    : m_ctrl(ctrl)
    , m_theme(MakeDefaultTheme(ctrl))
{
}

clRichTextMarkdownRenderer::Theme clRichTextMarkdownRenderer::MakeDefaultTheme(const wxWindow* win)
{
    Theme theme;
    theme.font = win != nullptr ? win->GetFont() : wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    theme.monospaceFont = wxFont(wxFontInfo(theme.font.GetFractionalPointSize()).Family(wxFONTFAMILY_TELETYPE));
    theme.text = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    theme.background = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    theme.link = wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT);
    theme.muted = wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
    // A little different from the background: lighter on a dark theme, darker on a light one
    const bool dark = IsDark(theme.background);
    theme.codeBackground = theme.background.ChangeLightness(dark ? 125 : 94);
    theme.border = theme.background.ChangeLightness(dark ? 160 : 75);
    theme.quote = theme.muted;
    return theme;
}

void clRichTextMarkdownRenderer::ResetState()
{
    m_container = nullptr;
    m_para = nullptr;
    m_lists.clear();
    m_quoteDepth = 0;
    m_headingLevel = 0;
    m_inCodeBlock = false;
    m_inHtmlBlock = false;
    m_pendingBreaks = 0;
    m_emphasis = m_strong = m_underline = m_strikethrough = m_code = 0;
    m_links.clear();
    m_itemPending = m_itemTask = m_itemTaskChecked = false;
    m_imageDepth = 0;
    m_imageUrl.clear();
    m_imageAlt.clear();
    m_table = nullptr;
    m_tableOwner = nullptr;
    m_cell = nullptr;
    m_tableColumns = 0;
    m_tableRows = 0;
    m_tableColumn = -1;
    m_inTableHeader = false;
    m_cellAlign = Align::Default;
}

bool clRichTextMarkdownRenderer::Render(const wxString& markdown)
{
    wxCHECK_MSG(m_ctrl != nullptr, false, "clRichTextMarkdownRenderer: no control");

    // Thaw() when we are done, whatever happens
    struct FreezeGuard {
        explicit FreezeGuard(wxRichTextCtrl* ctrl)
            : m_ctrl(ctrl)
        {
            m_ctrl->Freeze();
        }
        ~FreezeGuard() { m_ctrl->Thaw(); }
        wxRichTextCtrl* m_ctrl;
    } freeze_guard{m_ctrl};

    // The style of the text that has no style of its own
    wxRichTextAttr basic_style;
    basic_style.SetFont(m_theme.font);
    basic_style.SetTextColour(m_theme.text);
    m_ctrl->SetBackgroundColour(m_theme.background);
    m_ctrl->SetBasicStyle(basic_style);

    auto& buffer = m_ctrl->GetBuffer();
    // The caret can be inside a table cell (after a click), and the cell is deleted by Clear()
    m_ctrl->SetFocusObject(&buffer, false);
    // Forget the undo/redo commands too: they point to objects that are deleted now. It leaves one empty paragraph,
    // we want an empty buffer: our paragraphs are added at the end
    buffer.ResetAndClearCommands();
    buffer.Clear();
    ResetState();
    m_container = &buffer;

    const bool ok = Parse(markdown);

    // An empty control must have one (empty) paragraph
    if (buffer.GetChildCount() == 0) {
        buffer.Reset();
    }
    ResetState();

    buffer.UpdateRanges();
    buffer.Invalidate(wxRICHTEXT_ALL);
    m_ctrl->SetInsertionPoint(0);
    m_ctrl->Refresh();
    return ok;
}

// ---------------------------------------------------------------------------
// Attributes
// ---------------------------------------------------------------------------

wxRichTextAttr clRichTextMarkdownRenderer::BuildCharAttr() const
{
    const bool monospace = m_inCodeBlock || m_code > 0;
    wxFont font = monospace ? m_theme.monospaceFont : m_theme.font;

    if (m_headingLevel > 0) {
        const size_t index = std::min<size_t>(m_headingLevel, std::size(kHeadingScale)) - 1;
        font.SetFractionalPointSize(font.GetFractionalPointSize() * kHeadingScale[index]);
        font.SetWeight(wxFONTWEIGHT_BOLD);
    }
    if (m_strong > 0 || m_inTableHeader) {
        font.SetWeight(wxFONTWEIGHT_BOLD);
    }
    if (m_emphasis > 0) {
        font.SetStyle(wxFONTSTYLE_ITALIC);
    }
    if (m_underline > 0 || !m_links.empty()) {
        font.SetUnderlined(true);
    }
    if (m_strikethrough > 0) {
        font.SetStrikethrough(true);
    }

    wxRichTextAttr attr;
    attr.SetFont(font);
    attr.SetTextColour(m_links.empty() ? m_theme.text : m_theme.link);
    if (m_code > 0 && !m_inCodeBlock) {
        // Inline code. The background of a code block is the one of its paragraph
        attr.SetBackgroundColour(m_theme.codeBackground);
    }
    if (!m_links.empty()) {
        attr.SetURL(m_links.back());
    }
    return attr;
}

wxRichTextAttr clRichTextMarkdownRenderer::BuildParagraphAttr(bool firstInListItem) const
{
    wxRichTextAttr attr;

    const int depth = static_cast<int>(m_lists.size());
    const int quote_indent = m_quoteDepth * kQuoteIndent;
    int spacing_before = 0;
    int spacing_after = (!m_lists.empty() && m_lists.back().tight) ? kTightSpacing : kParagraphSpacing;

    if (depth > 0) {
        // The bullet of this level; the text starts after it
        const int bullet_position = quote_indent + (depth - 1) * kBulletIndent;
        if (firstInListItem) {
            attr.SetLeftIndent(bullet_position, kBulletSubIndent);
            const ListState& list = m_lists.back();
            if (m_itemTask) {
                // A task item has a check box (a character) instead of a bullet, see EnsureParagraph()
            } else if (list.ordered) {
                attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_ARABIC | wxTEXT_ATTR_BULLET_STYLE_PERIOD);
                attr.SetBulletNumber(static_cast<int>(list.nextNumber));
            } else {
                static const wxString kBulletNames[] = {"standard/circle", "standard/diamond", "standard/square"};
                attr.SetBulletStyle(wxTEXT_ATTR_BULLET_STYLE_STANDARD);
                attr.SetBulletName(kBulletNames[(depth - 1) % std::size(kBulletNames)]);
            }
        } else {
            // Another paragraph of the same item: it lines up with the text of the item
            attr.SetLeftIndent(bullet_position + kBulletSubIndent, 0);
        }
    } else if (quote_indent > 0) {
        attr.SetLeftIndent(quote_indent, 0);
    }

    if (m_quoteDepth > 0) {
        // A bar at the left side
        auto& left = attr.GetTextBoxAttr().GetBorder().GetLeft();
        left.SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
        left.SetColour(m_theme.quote);
        left.SetWidth(3, wxTEXT_ATTR_UNITS_PIXELS);
        attr.GetTextBoxAttr().GetPadding().GetLeft().SetValue(8, wxTEXT_ATTR_UNITS_PIXELS);
    }

    if (m_headingLevel > 0) {
        spacing_before = kHeadingSpacingBefore;
        spacing_after = kHeadingSpacingAfter;
    }

    if (m_inCodeBlock) {
        attr.SetBackgroundColour(m_theme.codeBackground);
        attr.SetLeftIndent(quote_indent + (depth > 0 ? depth * kBulletIndent : 0) + kCodeIndent, 0);
    }

    switch (m_cellAlign) {
    case Align::Left:
        attr.SetAlignment(wxTEXT_ALIGNMENT_LEFT);
        break;
    case Align::Center:
        attr.SetAlignment(wxTEXT_ALIGNMENT_CENTRE);
        break;
    case Align::Right:
        attr.SetAlignment(wxTEXT_ALIGNMENT_RIGHT);
        break;
    case Align::Default:
        break;
    }

    if (m_cell != nullptr) {
        // No gap under the text of a table cell: the cell has its own padding
        spacing_before = 0;
        spacing_after = 0;
    }

    attr.SetParagraphSpacingBefore(spacing_before);
    attr.SetParagraphSpacingAfter(spacing_after);
    return attr;
}

// ---------------------------------------------------------------------------
// Writing to the control
// ---------------------------------------------------------------------------

wxRichTextParagraph* clRichTextMarkdownRenderer::AddParagraph(wxRichTextAttr& attr)
{
    wxCHECK_MSG(m_container != nullptr, nullptr, "No container");
    m_container->AddParagraph(wxEmptyString, &attr);
    const size_t count = m_container->GetChildCount();
    return count == 0 ? nullptr : wxDynamicCast(m_container->GetChild(count - 1), wxRichTextParagraph);
}

wxRichTextParagraph* clRichTextMarkdownRenderer::EnsureParagraph()
{
    if (m_para != nullptr || m_container == nullptr) {
        return m_para;
    }

    const bool first_in_item = m_itemPending;
    m_itemPending = false;

    wxRichTextAttr attr = BuildParagraphAttr(first_in_item);
    m_para = AddParagraph(attr);
    if (m_para != nullptr && first_in_item && m_itemTask) {
        // The check box of a task item
        AppendText(m_itemTaskChecked ? wxString::FromUTF8("\xE2\x98\x91 ") : wxString::FromUTF8("\xE2\x98\x90 "),
                   BuildCharAttr());
    }
    return m_para;
}

void clRichTextMarkdownRenderer::AppendText(const wxString& text, const wxRichTextAttr& attr)
{
    if (text.empty()) {
        return;
    }
    wxRichTextParagraph* para = EnsureParagraph();
    if (para == nullptr) {
        return;
    }
    // The constructor copies the style
    wxRichTextAttr style = attr;
    para->AppendChild(new wxRichTextPlainText(text, para, &style));
}

void clRichTextMarkdownRenderer::AppendMultilineText(const wxString& text, const wxRichTextAttr& attr)
{
    // The text of a code block comes in pieces, each line ends with '\n'. The last '\n' must not be a line break
    // (it would add an empty line), so a new line is written only when more text follows
    wxString::size_type start = 0;
    while (start <= text.length()) {
        const auto end = text.find('\n', start);
        const wxString line = text.substr(start, end == wxString::npos ? wxString::npos : end - start);
        if (!line.empty()) {
            if (m_pendingBreaks > 0) {
                AppendText(wxString(wxRichTextLineBreakChar, m_pendingBreaks), attr);
                m_pendingBreaks = 0;
            }
            AppendText(line, attr);
        }
        if (end == wxString::npos) {
            break;
        }
        ++m_pendingBreaks;
        start = end + 1;
    }
}

wxImage clRichTextMarkdownRenderer::LoadImage(const wxString& url) const
{
    const wxString path = LocalImagePath(url);
    if (path.empty()) {
        return wxNullImage;
    }

    wxFileName fn(path);
    if (fn.IsRelative() && !m_baseDir.empty()) {
        fn.MakeAbsolute(m_baseDir);
    }
    if (!fn.FileExists()) {
        return wxNullImage;
    }

    wxImage image;
    {
        // Do not show a message box for an image that is broken
        wxLogNull no_log;
        if (!image.LoadFile(fn.GetFullPath())) {
            return wxNullImage;
        }
    }
    if (image.GetWidth() > kMaxImageWidth) {
        const int height = image.GetHeight() * kMaxImageWidth / image.GetWidth();
        image.Rescale(kMaxImageWidth, std::max(height, 1), wxIMAGE_QUALITY_HIGH);
    }
    return image;
}

void clRichTextMarkdownRenderer::AppendImage(const wxString& url, const wxString& alt)
{
    wxImage image = LoadImage(url);
    if (!image.IsOk()) {
        // Show the alternative text instead, or the address
        wxRichTextAttr attr = BuildCharAttr();
        attr.SetTextColour(m_theme.muted);
        AppendText("[" + (alt.empty() ? url : alt) + "]", attr);
        return;
    }

    wxRichTextParagraph* para = EnsureParagraph();
    if (para == nullptr) {
        return;
    }
    // The style of the image has the link, if the image is inside a link
    wxRichTextAttr attr = BuildCharAttr();
    para->AppendChild(new wxRichTextImage(image, para, &attr));
}

// ---------------------------------------------------------------------------
// Tables
// ---------------------------------------------------------------------------

void clRichTextMarkdownRenderer::EnterTableCell(bool header, Align align)
{
    if (m_table == nullptr) {
        return;
    }
    ++m_tableColumn;
    m_inTableHeader = header;
    m_cellAlign = align;

    m_cell = m_table->GetCell(m_tableRows - 1, m_tableColumn);
    if (m_cell == nullptr) {
        return;
    }

    // Remove what the table put in the cell, our paragraphs are added when the content arrives
    m_cell->Clear();

    wxRichTextAttr cell_attr;
    SetBorders(cell_attr, m_theme.border, 1);
    auto& padding = cell_attr.GetTextBoxAttr().GetPadding();
    padding.GetLeft().SetValue(kCellPadding, wxTEXT_ATTR_UNITS_PIXELS);
    padding.GetRight().SetValue(kCellPadding, wxTEXT_ATTR_UNITS_PIXELS);
    padding.GetTop().SetValue(kCellPadding, wxTEXT_ATTR_UNITS_PIXELS);
    padding.GetBottom().SetValue(kCellPadding, wxTEXT_ATTR_UNITS_PIXELS);
    if (header) {
        cell_attr.SetBackgroundColour(m_theme.codeBackground);
    }
    m_cell->SetAttributes(cell_attr);

    m_container = m_cell;
    m_para = nullptr;
}

void clRichTextMarkdownRenderer::LeaveTableCell()
{
    if (m_cell != nullptr && m_cell->GetChildCount() == 0) {
        // A cell without content still needs a paragraph
        wxRichTextAttr attr;
        m_cell->AddParagraph(wxEmptyString, &attr);
    }
    m_cell = nullptr;
    m_container = m_tableOwner;
    m_para = nullptr;
    m_inTableHeader = false;
    m_cellAlign = Align::Default;
}

// ---------------------------------------------------------------------------
// The parser callbacks
// ---------------------------------------------------------------------------

void clRichTextMarkdownRenderer::OnEnterBlock(Block block, const BlockInfo& info)
{
    // A list item that starts with a block that is not text (a nested list, a table, a rule) has no text of its own:
    // write its bullet now. Otherwise, the next paragraph (it can be the one of a table cell) takes the bullet
    if (m_itemPending && (block == Block::UnorderedList || block == Block::OrderedList || block == Block::Table ||
                          block == Block::HorizontalRule)) {
        EnsureParagraph();
    }

    // A block ends the paragraph of the block before it. (A paragraph that belongs to a list item can have a list
    // inside it: the text, then the nested list)
    m_para = nullptr;
    m_pendingBreaks = 0;

    switch (block) {
    case Block::Document:
        break;
    case Block::Quote:
        ++m_quoteDepth;
        break;
    case Block::UnorderedList:
    case Block::OrderedList: {
        ListState list;
        list.ordered = block == Block::OrderedList;
        list.tight = info.tight;
        list.nextNumber = info.start;
        m_lists.push_back(list);
        break;
    }
    case Block::ListItem:
        m_itemPending = true;
        m_itemTask = info.isTask;
        m_itemTaskChecked = info.taskChecked;
        break;
    case Block::HorizontalRule: {
        wxRichTextAttr attr = BuildParagraphAttr(false);
        auto& bottom = attr.GetTextBoxAttr().GetBorder().GetBottom();
        bottom.SetStyle(wxTEXT_BOX_ATTR_BORDER_SOLID);
        bottom.SetColour(m_theme.border);
        bottom.SetWidth(1, wxTEXT_ATTR_UNITS_PIXELS);
        AddParagraph(attr);
        break;
    }
    case Block::Heading:
        m_headingLevel = info.level;
        break;
    case Block::CodeBlock:
        m_inCodeBlock = true;
        m_pendingBreaks = 0;
        break;
    case Block::HtmlBlock:
        m_inHtmlBlock = true;
        m_pendingBreaks = 0;
        break;
    case Block::Paragraph:
        break;
    case Block::Table: {
        if (m_container == nullptr) {
            break;
        }
        m_tableOwner = m_container;
        m_table = new wxRichTextTable(m_container);
        wxRichTextAttr table_attr;
        table_attr.GetTextBoxAttr().GetWidth().SetValue(100, wxTEXT_ATTR_UNITS_PERCENTAGE);
        m_table->SetAttributes(table_attr);
        m_container->AppendChild(m_table);
        m_tableColumns = info.columns;
        m_tableRows = 0;
        break;
    }
    case Block::TableHead:
    case Block::TableBody:
        break;
    case Block::TableRow:
        if (m_table != nullptr) {
            // The table is created with its first row, the other rows are added
            if (m_tableRows == 0) {
                m_table->CreateTable(1, static_cast<int>(m_tableColumns));
            } else {
                m_table->AddRows(m_table->GetRowCount(), 1);
            }
            ++m_tableRows;
            m_tableColumn = -1;
        }
        break;
    case Block::TableHeaderCell:
        EnterTableCell(true, info.align);
        break;
    case Block::TableCell:
        EnterTableCell(false, info.align);
        break;
    }
}

void clRichTextMarkdownRenderer::OnLeaveBlock(Block block, const BlockInfo& info)
{
    wxUnusedVar(info);

    // An empty list item: it still has a bullet
    if (block == Block::ListItem && m_itemPending) {
        EnsureParagraph();
    }

    m_para = nullptr;
    m_pendingBreaks = 0;

    switch (block) {
    case Block::Quote:
        m_quoteDepth = std::max(0, m_quoteDepth - 1);
        break;
    case Block::UnorderedList:
    case Block::OrderedList:
        if (!m_lists.empty()) {
            m_lists.pop_back();
        }
        break;
    case Block::ListItem:
        m_itemPending = false;
        if (!m_lists.empty()) {
            ++m_lists.back().nextNumber;
        }
        break;
    case Block::Heading:
        m_headingLevel = 0;
        break;
    case Block::CodeBlock:
        m_inCodeBlock = false;
        m_pendingBreaks = 0;
        break;
    case Block::HtmlBlock:
        m_inHtmlBlock = false;
        m_pendingBreaks = 0;
        break;
    case Block::Table:
        m_table = nullptr;
        m_container = m_tableOwner;
        m_tableOwner = nullptr;
        break;
    case Block::TableHeaderCell:
    case Block::TableCell:
        LeaveTableCell();
        break;
    case Block::Document:
    case Block::HorizontalRule:
    case Block::Paragraph:
    case Block::TableHead:
    case Block::TableBody:
    case Block::TableRow:
        break;
    }
}

void clRichTextMarkdownRenderer::OnEnterSpan(Span span, const SpanInfo& info)
{
    switch (span) {
    case Span::Emphasis:
        ++m_emphasis;
        break;
    case Span::Strong:
        ++m_strong;
        break;
    case Span::Underline:
        ++m_underline;
        break;
    case Span::Strikethrough:
        ++m_strikethrough;
        break;
    case Span::Code:
    case Span::LatexMath:
    case Span::LatexMathDisplay:
        ++m_code;
        break;
    case Span::Link:
    case Span::WikiLink:
        m_links.push_back(info.url);
        break;
    case Span::Image:
        if (++m_imageDepth == 1) {
            // The text of the image is its alternative text, it is kept until the end of the image
            m_imageUrl = info.url;
            m_imageAlt.clear();
        }
        break;
    }
}

void clRichTextMarkdownRenderer::OnLeaveSpan(Span span, const SpanInfo& info)
{
    wxUnusedVar(info);
    switch (span) {
    case Span::Emphasis:
        m_emphasis = std::max(0, m_emphasis - 1);
        break;
    case Span::Strong:
        m_strong = std::max(0, m_strong - 1);
        break;
    case Span::Underline:
        m_underline = std::max(0, m_underline - 1);
        break;
    case Span::Strikethrough:
        m_strikethrough = std::max(0, m_strikethrough - 1);
        break;
    case Span::Code:
    case Span::LatexMath:
    case Span::LatexMathDisplay:
        m_code = std::max(0, m_code - 1);
        break;
    case Span::Link:
    case Span::WikiLink:
        if (!m_links.empty()) {
            m_links.pop_back();
        }
        break;
    case Span::Image:
        if (m_imageDepth > 0 && --m_imageDepth == 0) {
            AppendImage(m_imageUrl, m_imageAlt);
            m_imageUrl.clear();
            m_imageAlt.clear();
        }
        break;
    }
}

void clRichTextMarkdownRenderer::OnText(TextType type, const wxString& text)
{
    if (m_imageDepth > 0) {
        // The alternative text of an image: only the text, without the format
        switch (type) {
        case TextType::Break:
        case TextType::SoftBreak:
            m_imageAlt << ' ';
            break;
        case TextType::Html:
            // Tags do not belong to the alternative text
            break;
        default:
            m_imageAlt << text;
            break;
        }
        return;
    }

    switch (type) {
    case TextType::Normal:
        AppendText(text, BuildCharAttr());
        break;
    case TextType::Break:
        AppendText(wxString(wxRichTextLineBreakChar), BuildCharAttr());
        break;
    case TextType::SoftBreak:
        AppendText(" ", BuildCharAttr());
        break;
    case TextType::Code:
    case TextType::LatexMath:
        AppendMultilineText(text, BuildCharAttr());
        break;
    case TextType::Html: {
        // Raw HTML is not rendered: show it as text, dimmed
        wxRichTextAttr attr = BuildCharAttr();
        attr.SetTextColour(m_theme.muted);
        AppendMultilineText(text, attr);
        break;
    }
    }
}
