#include "clRichTextMarkdownRenderer.hpp"

#include "ColoursAndFontsManager.h"
#include "globals.h"
#include "lexer_configuration.h"

#include <algorithm>
#include <iterator>
#include <wx/dcclient.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/log.h>
#include <wx/regex.h>
#include <wx/settings.h>
#include <wx/uri.h>
#include <wx/wupdlock.h>

namespace
{
// Sizes of the rich text control are in tenths of a millimetre
constexpr int kBulletIndent = 100;   ///< Each level of a list moves the bullet by this
constexpr int kBulletSubIndent = 60; ///< The text of an item starts this far after its bullet
constexpr int kQuoteIndent = 80;
constexpr int kCodePadding = 8; ///< In pixels
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

/// The foreground (or the background) colour of the style `style` (a `wxSTC_*` value) of `lexer`.
/// `fallback` if there is no lexer, no such style, or the colour is not valid
wxColour StyleColour(const LexerConf::Ptr_t& lexer, int style, bool foreground, const wxColour& fallback)
{
    if (!lexer) {
        return fallback;
    }
    const StyleProperty& property = lexer->GetProperty(style);
    if (property.IsNull()) {
        return fallback;
    }
    const wxColour colour(foreground ? property.GetFgColour() : property.GetBgColour());
    return colour.IsOk() ? colour : fallback;
}

/// The name that a heading has in a link to it ("#name"), as GitHub makes it: lower case, a space is '-', letters,
/// digits, '-' and '_' stay, and the rest of the punctuation is removed
wxString MakeAnchorName(const wxString& heading)
{
    wxString name;
    for (const wxUniChar ch : heading.Lower()) {
        if (ch == ' ') {
            name << '-';
        } else if (ch == '-' || ch == '_' || ch.GetValue() > 127 || wxIsalnum(ch.GetValue())) {
            // The characters that are not ASCII (letters of other languages) stay
            name << ch;
        }
    }
    return name;
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

clMarkdownTextCtrl::clMarkdownTextCtrl(
    wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style)
    : wxRichTextCtrl(parent, id, value, pos, size, style)
{
    // Bound after the control's own handlers are set, so this one runs first
    Bind(wxEVT_MOTION, &clMarkdownTextCtrl::OnMouseMove, this);
    Bind(wxEVT_LEAVE_WINDOW, [this](wxMouseEvent& event) {
        event.Skip();
        SetHoveredUrl(wxEmptyString);
    });
}

bool clMarkdownTextCtrl::ProcessMouseMovement(wxRichTextParagraphLayoutBox* container,
                                              wxRichTextObject* obj,
                                              long position,
                                              const wxPoint& pos)
{
    // The control changes the cursor over a link. We also show the address of the link
    const bool result = wxRichTextCtrl::ProcessMouseMovement(container, obj, position, pos);
    m_overText = true;

    wxString url;
    wxRichTextAttr attr;
    if (container != nullptr && GetStyle(position, attr, container) && attr.HasFlag(wxTEXT_ATTR_URL)) {
        url = attr.GetURL();
    }
    SetHoveredUrl(url);
    return result;
}

void clMarkdownTextCtrl::OnMouseMove(wxMouseEvent& event)
{
    event.Skip();

    // The control calls ProcessMouseMovement() only when the mouse is over some text. When it is not (the empty space
    // around the text), the tooltip of the link that the mouse left must go: check after the control handled the event
    m_overText = false;
    if (!m_checkQueued) {
        m_checkQueued = true;
        CallAfter([this]() {
            m_checkQueued = false;
            if (!m_overText) {
                SetHoveredUrl(wxEmptyString);
            }
        });
    }
}

void clMarkdownTextCtrl::SetHoveredUrl(const wxString& url)
{
    if (url == m_hoveredUrl) {
        // Do not touch the tooltip while the mouse moves over the same link: it would restart the delay
        return;
    }
    m_hoveredUrl = url;
    if (url.empty()) {
        UnsetToolTip();
    } else {
        SetToolTip(url);
    }
}

void clMarkdownTextCtrl::Copy()
{
    wxString text = GetStringSelection();
    if (text.empty()) {
        return;
    }
    // A line break of a paragraph is the character wxRichTextLineBreakChar (GS), make it a new line
    text.Replace(wxString(wxRichTextLineBreakChar), "\n");
    ::CopyToClipboard(text);
}

wxString clRichTextMarkdownRenderer::LocalPathFromUrl(const wxString& url)
{
    wxString path = url;

    // A scheme is 2 or more characters followed by ':'. One character is a drive letter ("C:\dir")
    static const wxRegEx kScheme("^[A-Za-z][A-Za-z0-9+.-]+:", wxRE_EXTENDED);
    if (kScheme.Matches(path)) {
        // The scheme is not case sensitive: "FILE://" is the same
        if (!path.Lower().StartsWith("file://")) {
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
    // are all network paths (also with more than 2 separators; also the device paths "\\?\" and "\\.\"). Check a copy
    // that has only '/'
    wxString normalized = path;
    normalized.Replace("\\", "/");
    if (normalized.StartsWith("//")) {
        return wxEmptyString;
    }
    return path;
}

clRichTextMarkdownRenderer::clRichTextMarkdownRenderer(wxRichTextCtrl* ctrl)
    : m_ctrl(ctrl)
    , m_theme(MakeDefaultTheme(ctrl))
{
}

clRichTextMarkdownRenderer::Theme clRichTextMarkdownRenderer::MakeDefaultTheme(const wxWindow* win)
{
    const wxFont font = win != nullptr ? win->GetFont() : wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
    const wxFont monospace_font(wxFontInfo(font.GetFractionalPointSize()).Family(wxFONTFAMILY_TELETYPE));
    return MakeTheme(font,
                     monospace_font,
                     wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT),
                     wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
}

clRichTextMarkdownRenderer::Theme clRichTextMarkdownRenderer::MakeTheme(const wxFont& font,
                                                                        const wxFont& monospaceFont,
                                                                        const wxColour& text,
                                                                        const wxColour& background)
{
    Theme theme;
    theme.font = font;
    theme.monospaceFont = monospaceFont;
    theme.text = text;
    theme.background = background;

    const bool dark = IsDark(background);
    // A blue that can be read on the background
    theme.link = dark ? wxColour(102, 178, 255) : wxColour(0, 102, 204);
    // Between the text and the background
    theme.muted = wxColour((text.Red() * 6 + background.Red() * 4) / 10,
                           (text.Green() * 6 + background.Green() * 4) / 10,
                           (text.Blue() * 6 + background.Blue() * 4) / 10);
    // Colours that are not the colour of the text or of the links
    theme.inlineCode = dark ? wxColour(240, 160, 110) : wxColour(176, 56, 56);

    // A little different from the background: lighter on a dark theme, darker on a light one
    theme.codeBackground = background.ChangeLightness(dark ? 105 : 95);

    // The other colours are the ones of the C++ lexer of the current theme, so they fit the colours of the editor
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
    if (!lexer) {
        lexer = ColoursAndFontsManager::Get().GetLexer("text");
    }
    // The headings have the colour of a class, the bullets the colour of a number
    theme.heading =
        StyleColour(lexer, wxSTC_C_GLOBALCLASS, true, dark ? wxColour(120, 200, 170) : wxColour(0, 110, 90));
    theme.bullet = StyleColour(lexer, wxSTC_C_NUMBER, true, theme.heading);
    // The border is the background colour of the default style, a little lighter on a dark theme and a little darker
    // on a light one
    const wxColour default_background = StyleColour(lexer, wxSTC_C_DEFAULT, false, background);
    theme.border = default_background.ChangeLightness(IsDark(default_background) ? 110 : 90);
    theme.quote = theme.muted;
    return theme;
}

void clRichTextMarkdownRenderer::ResetState()
{
    m_container = nullptr;
    m_para = nullptr;
    m_codeOwner = nullptr;
    m_lists.clear();
    m_quoteDepth = 0;
    m_headingLevel = 0;
    m_headingPara = nullptr;
    m_headingText.clear();
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

    // No drawing while the content is built
    wxWindowUpdateLocker locker(m_ctrl);

    // The scroll unit of the control is a number of pixels, and it is only 5 by default: a turn of the mouse wheel is
    // 3 units, so the document moves very slowly. Use the height of one line of text: a scroll bar step is one line,
    // and a turn of the mouse wheel (3 steps) is 3 lines
    {
        wxClientDC dc(m_ctrl);
        dc.SetFont(m_theme.font);
        m_ctrl->SetLineHeight(std::max(1, dc.GetCharHeight()));
    }

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
    // The paragraphs of the headings are deleted with the buffer
    m_anchors.clear();
    m_anchorCount.clear();
    m_headings.clear();
    buffer.Clear();
    ResetState();
    m_container = &buffer;

    // The margins are attributes of the document. (They are set after the commands are reset, to be sure that they
    // are not lost)
    m_ctrl->SetMargins(m_ctrl->FromDIP(m_marginHorizontal), m_ctrl->FromDIP(m_marginVertical));

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

bool clRichTextMarkdownRenderer::GoToAnchor(const wxString& anchor)
{
    // The name in a link can be percent-encoded, and it is not case sensitive
    const auto where = m_anchors.find(wxURI::Unescape(anchor).Lower());
    if (m_ctrl == nullptr || where == m_anchors.end()) {
        return false;
    }

    // The scroll unit of the control is one line (see Render()): scroll so that the heading is at the top
    const int unit = std::max(1, m_ctrl->GetLineHeight());
    m_ctrl->Scroll(-1, std::max(0, where->second->GetRect().y / unit));
    return true;
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
    const bool inline_code = m_code > 0 && !m_inCodeBlock;
    if (!m_links.empty()) {
        attr.SetTextColour(m_theme.link);
    } else if (inline_code) {
        attr.SetTextColour(m_theme.inlineCode);
    } else if (m_headingLevel > 0) {
        attr.SetTextColour(m_theme.heading);
    } else {
        attr.SetTextColour(m_theme.text);
    }
    if (inline_code) {
        // The background of a code block is the one of its box, see OnEnterBlock()
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

    if (m_inCodeBlock) {
        // The paragraphs of a code block are inside a box (see OnEnterBlock()). The box has the background and the
        // padding, and the paragraph that holds the box has the indent, so here there is nothing to add
        attr.SetParagraphSpacingBefore(0);
        attr.SetParagraphSpacingAfter(0);
        return attr;
    }

    const int depth = static_cast<int>(m_lists.size());
    const int quote_indent = m_quoteDepth * kQuoteIndent;
    int spacing_before = 0;
    int spacing_after = (!m_lists.empty() && m_lists.back().tight) ? kTightSpacing : kParagraphSpacing;

    if (depth > 0) {
        // The bullet of this level; the text starts after it
        const int bullet_position = quote_indent + (depth - 1) * kBulletIndent;
        if (firstInListItem) {
            attr.SetLeftIndent(bullet_position, kBulletSubIndent);
            // The bullet (or the number) is drawn with the text colour of the paragraph. The text of the item has
            // its own colour: every piece of text has it
            attr.SetTextColour(m_theme.bullet);
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
    if (m_para != nullptr && m_headingLevel > 0 && m_headingPara == nullptr && m_container == &m_ctrl->GetBuffer()) {
        // The paragraph of a heading of the document: a link can go to it, see GoToAnchor(). (The text of the heading
        // is not cleared here: OnText() adds the text of the heading before it creates the paragraph)
        m_headingPara = m_para;
    }
    if (m_para != nullptr && first_in_item && m_itemTask) {
        // The check box of a task item: it takes the place of the bullet, so it has the colour of the bullets
        wxRichTextAttr check_box_attr = BuildCharAttr();
        check_box_attr.SetTextColour(m_theme.bullet);
        AppendText(m_itemTaskChecked ? wxString::FromUTF8("\xE2\x98\x91 ") : wxString::FromUTF8("\xE2\x98\x90 "),
                   check_box_attr);
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

wxImage clRichTextMarkdownRenderer::LoadLocalImage(const wxString& url) const
{
    const wxString path = LocalPathFromUrl(url);
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
    wxImage image = LoadLocalImage(url);
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
                          block == Block::HorizontalRule || block == Block::CodeBlock)) {
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
        m_headingPara = nullptr;
        m_headingText.clear();
        break;
    case Block::CodeBlock: {
        if (m_container == nullptr) {
            break;
        }
        // The background must cover the whole width, so the code is in a box that is 100% wide, and the box is in a
        // paragraph. (The background of a paragraph covers only its text). That paragraph has the indent of the list
        // or of the quote, and the spacing after the block
        wxRichTextAttr holder_attr = BuildParagraphAttr(false);
        wxRichTextParagraph* holder = AddParagraph(holder_attr);
        if (holder == nullptr) {
            break;
        }

        auto* box = new wxRichTextBox(holder);
        wxRichTextAttr box_attr;
        box_attr.GetTextBoxAttr().GetWidth().SetValue(100, wxTEXT_ATTR_UNITS_PERCENTAGE);
        box_attr.SetBackgroundColour(m_theme.codeBackground);
        SetBorders(box_attr, m_theme.border, 1);
        auto& padding = box_attr.GetTextBoxAttr().GetPadding();
        padding.GetLeft().SetValue(kCodePadding, wxTEXT_ATTR_UNITS_PIXELS);
        padding.GetRight().SetValue(kCodePadding, wxTEXT_ATTR_UNITS_PIXELS);
        padding.GetTop().SetValue(kCodePadding, wxTEXT_ATTR_UNITS_PIXELS);
        padding.GetBottom().SetValue(kCodePadding, wxTEXT_ATTR_UNITS_PIXELS);
        box->SetAttributes(box_attr);
        holder->AppendChild(box);

        m_codeOwner = m_container;
        m_container = box;
        m_inCodeBlock = true;
        m_pendingBreaks = 0;
        break;
    }
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
        // A table is an object that lives inside a paragraph (as WriteTable() of the control does). It must not be
        // added directly to the document: the document lays out its children as paragraphs, and it would not draw it
        wxRichTextAttr holder_attr = BuildParagraphAttr(false);
        wxRichTextParagraph* holder = AddParagraph(holder_attr);
        if (holder == nullptr) {
            break;
        }
        m_tableOwner = m_container;
        m_table = new wxRichTextTable(holder);
        wxRichTextAttr table_attr;
        table_attr.GetTextBoxAttr().GetWidth().SetValue(100, wxTEXT_ATTR_UNITS_PERCENTAGE);
        m_table->SetAttributes(table_attr);
        holder->AppendChild(m_table);
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
        if (m_headingPara != nullptr) {
            // The name of the heading for the links ("#name"). A name that is used again gets a number
            wxString name = MakeAnchorName(m_headingText);
            if (!name.empty()) {
                const int used = m_anchorCount[name]++;
                if (used > 0) {
                    name << '-' << used;
                }
                m_anchors[name] = m_headingPara;
                m_headings.push_back({static_cast<int>(m_headingLevel), m_headingText.Strip(wxString::both), name});
            }
        }
        m_headingPara = nullptr;
        m_headingText.clear();
        m_headingLevel = 0;
        break;
    case Block::CodeBlock:
        if (m_codeOwner != nullptr) {
            // An empty code block: the box still needs a paragraph
            if (m_container != nullptr && m_container->GetChildCount() == 0) {
                wxRichTextAttr attr;
                m_container->AddParagraph(wxEmptyString, &attr);
            }
            m_container = m_codeOwner;
            m_codeOwner = nullptr;
        }
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
        if (m_headingLevel > 0) {
            m_headingText << text;
        }
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
        if (m_headingLevel > 0) {
            // An inline code in a heading is part of the name of the heading
            m_headingText << text;
        }
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
