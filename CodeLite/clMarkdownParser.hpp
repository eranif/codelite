#pragma once

#include "codelite_exports.h"

#include <wx/string.h>

/// A Markdown parser: a C++ wrapper for md4c (https://github.com/mity/md4c).
///
/// It works like a SAX parser: `Parse()` reads the document and calls the `On...()` methods, in document order, for each
/// block (paragraph, list, ...), each span (emphasis, link, ...) and each piece of text. Derive from this class and
/// override the methods you need, for example to build a wxRichTextCtrl buffer or HTML.
///
/// The enter/leave calls are always balanced and nested, so a derived class can use `Begin...()`/`End...()` style APIs.
///
/// Example:
///
///     class MyRenderer : public clMarkdownParser
///     {
///     protected:
///         void OnEnterSpan(Span span, const SpanInfo& info) override { ... }
///         void OnText(TextType type, const wxString& text) override { ... }
///     };
///     MyRenderer renderer;
///     renderer.Parse(markdown_text);
class WXDLLIMPEXP_CL clMarkdownParser
{
public:
    /// The block elements of a document
    enum class Block {
        Document,       ///< The whole document, the first block (and the last)
        Quote,          ///< > quote
        UnorderedList,  ///< See BlockInfo::tight
        OrderedList,    ///< See BlockInfo::tight and BlockInfo::start
        ListItem,       ///< See BlockInfo::isTask and BlockInfo::taskChecked
        HorizontalRule, ///< ---
        Heading,        ///< See BlockInfo::level
        CodeBlock,      ///< See BlockInfo::lang. The text lines end with '\n' (it comes as TextType::Code)
        HtmlBlock,      ///< Raw HTML. The text comes as TextType::Html
        Paragraph,
        Table,           ///< See BlockInfo::columns. Only with Flags::Tables
        TableHead,       ///< The header rows
        TableBody,       ///< The body rows
        TableRow,        ///< A row of the table
        TableHeaderCell, ///< A cell of the header. See BlockInfo::align
        TableCell,       ///< A cell of the body. See BlockInfo::align
    };

    /// The inline elements
    enum class Span {
        Emphasis,         ///< *text*
        Strong,           ///< **text**
        Link,             ///< See SpanInfo::url, SpanInfo::title and SpanInfo::isAutolink
        Image,            ///< See SpanInfo::url and SpanInfo::title. The alternative text comes as text (it can have spans)
        Code,             ///< `code`. The text comes as TextType::Code
        Strikethrough,    ///< ~~text~~. Only with Flags::Strikethrough
        LatexMath,        ///< $math$. Only with Flags::LatexMath. The text comes as TextType::LatexMath
        LatexMathDisplay, ///< $$math$$. Only with Flags::LatexMath. The text comes as TextType::LatexMath
        WikiLink,         ///< [[target]]. Only with Flags::WikiLinks. See SpanInfo::url
        Underline,        ///< Only with Flags::Underline
    };

    enum class TextType {
        Normal,    ///< Plain text. The entities (for example &amp;) are already decoded
        Break,     ///< A hard line break. The text is empty
        SoftBreak, ///< A new line in the source that is not a real line break. The text is empty
        Code,      ///< The text of a code block or of a code span. A code block has '\n' at the end of each line
        Html,      ///< Raw HTML
        LatexMath, ///< The text of an equation
    };

    enum class Align {
        Default,
        Left,
        Center,
        Right,
    };

    /// Extensions of CommonMark. A bit mask
    enum Flags : unsigned {
        kNone = 0,
        kTables = 1 << 0,
        kStrikethrough = 1 << 1,
        kTaskLists = 1 << 2,
        /// Recognize URLs, www. addresses and e-mails as links, even without '<' and '>'
        kPermissiveAutolinks = 1 << 3,
        /// Enable Span::Underline (and '_' does not mean emphasis anymore)
        kUnderline = 1 << 4,
        kLatexMath = 1 << 5,
        kWikiLinks = 1 << 6,
        /// Do not recognize raw HTML (as blocks and as spans), keep it as text
        kNoHtml = 1 << 7,
        /// Every new line in the source is a hard line break
        kHardSoftBreaks = 1 << 8,
        /// What GitHub supports
        kGitHub = kTables | kStrikethrough | kTaskLists | kPermissiveAutolinks,
    };

    /// More details of a block. Only the fields of the block type that are described are set
    struct BlockInfo {
        /// Heading: the level, 1 to 6
        unsigned level = 0;
        /// UnorderedList, OrderedList: a tight list has no blank lines between its items
        bool tight = false;
        /// OrderedList: the number of the first item
        unsigned start = 1;
        /// ListItem: is it an item of a task list ("- [ ] todo")?
        bool isTask = false;
        /// ListItem: is the task done ("- [x] done")?
        bool taskChecked = false;
        /// CodeBlock: the language of a fenced code block (the first word after the fence, can be empty)
        wxString lang;
        /// CodeBlock: everything after the fence, including the language (can be empty)
        wxString info;
        /// CodeBlock: is it a fenced block (and not an indented one)?
        bool fenced = false;
        /// Table: the number of columns
        unsigned columns = 0;
        /// TableHeaderCell, TableCell: the alignment of the column
        Align align = Align::Default;
    };

    /// More details of a span. Only the fields of the span type that are described are set
    struct SpanInfo {
        /// Link: the address. Image: the address of the image. WikiLink: the target
        wxString url;
        /// Link, Image: the title (can be empty)
        wxString title;
        /// Link: is it an autolink (<http://...> or a permissive one)?
        bool isAutolink = false;
    };

    clMarkdownParser() = default;
    virtual ~clMarkdownParser() = default;

    /// Copying is not allowed: a parser may hold the state of a document
    clMarkdownParser(const clMarkdownParser&) = delete;
    clMarkdownParser& operator=(const clMarkdownParser&) = delete;

    /// The extensions that are enabled. Default: `kGitHub`
    void SetFlags(unsigned flags) { m_flags = flags; }
    unsigned GetFlags() const { return m_flags; }

    /// Parse `markdown` and call the `On...()` methods.
    /// \return true on success. false if the parser failed (for example, not enough memory) or if `Abort()` was called
    bool Parse(const wxString& markdown);

protected:
    /// A block starts. The blocks are nested: every OnEnterBlock() has a matching OnLeaveBlock()
    virtual void OnEnterBlock(Block block, const BlockInfo& info);
    virtual void OnLeaveBlock(Block block, const BlockInfo& info);
    /// A span starts. Spans are nested too
    virtual void OnEnterSpan(Span span, const SpanInfo& info);
    virtual void OnLeaveSpan(Span span, const SpanInfo& info);
    /// A piece of text. It can be split: one paragraph can come as many OnText() calls
    virtual void OnText(TextType type, const wxString& text);

    /// Stop the parsing. Can be called from the `On...()` methods. `Parse()` returns false
    void Abort() { m_aborted = true; }

private:
    struct Impl;
    friend struct Impl;

    unsigned m_flags = kGitHub;
    bool m_aborted = false;
};
