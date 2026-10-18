#include "clMarkdownParser.hpp"

#include <limits>
#include <md4c.h>

// entity.h is part of md4c. It is a C header without the `extern "C"` guard
extern "C" {
#include <entity.h>
}

namespace
{
constexpr unsigned kMaxCodepoint = 0x10FFFF;
constexpr unsigned kReplacementChar = 0xFFFD;

/// A string with the Unicode character `codepoint`. An invalid codepoint becomes U+FFFD
wxString FromCodepoint(unsigned codepoint)
{
    const bool is_surrogate = codepoint >= 0xD800 && codepoint <= 0xDFFF;
    if (codepoint == 0 || codepoint > kMaxCodepoint || is_surrogate) {
        codepoint = kReplacementChar;
    }
    return wxString(wxUniChar(codepoint));
}

int HexValue(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }
    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }
    return 0;
}
} // namespace

/// The part that knows md4c: the callbacks and the conversion of its types.
/// It is a friend of clMarkdownParser, so the md4c types do not show in the header
struct clMarkdownParser::Impl {
    static wxString ToString(const MD_CHAR* text, MD_SIZE size) { return wxString::FromUTF8(text, size); }

    /// Decode an entity as md4c reports it: "&amp;", "&#35;" or "&#x23;". An unknown entity is returned as is
    static wxString DecodeEntity(const MD_CHAR* text, MD_SIZE size)
    {
        if (size > 3 && text[1] == '#') {
            // A numeric entity. Stop adding digits when the value is too big, to not overflow
            const bool is_hex = text[2] == 'x' || text[2] == 'X';
            unsigned codepoint = 0;
            for (MD_SIZE i = is_hex ? 3 : 2; i < size - 1 && codepoint <= kMaxCodepoint; ++i) {
                codepoint = is_hex ? codepoint * 16 + HexValue(text[i]) : codepoint * 10 + (text[i] - '0');
            }
            return FromCodepoint(codepoint);
        }

        // A named entity
        if (const ENTITY* entity = entity_lookup(text, size)) {
            wxString decoded = FromCodepoint(entity->codepoints[0]);
            if (entity->codepoints[1] != 0) {
                decoded << FromCodepoint(entity->codepoints[1]);
            }
            return decoded;
        }
        return ToString(text, size);
    }

    /// An attribute (an address, a title, ...) is made of pieces of different types, some of them are entities
    static wxString ToString(const MD_ATTRIBUTE& attr)
    {
        wxString result;
        for (unsigned i = 0; attr.substr_offsets[i] < attr.size; ++i) {
            const MD_OFFSET offset = attr.substr_offsets[i];
            const MD_SIZE size = attr.substr_offsets[i + 1] - offset;
            const MD_CHAR* text = attr.text + offset;
            switch (attr.substr_types[i]) {
            case MD_TEXT_NULLCHAR:
                result << FromCodepoint(kReplacementChar);
                break;
            case MD_TEXT_ENTITY:
                result << DecodeEntity(text, size);
                break;
            default:
                result << ToString(text, size);
                break;
            }
        }
        return result;
    }

    static Block ToBlock(MD_BLOCKTYPE type)
    {
        switch (type) {
        case MD_BLOCK_DOC:
            return Block::Document;
        case MD_BLOCK_QUOTE:
            return Block::Quote;
        case MD_BLOCK_UL:
            return Block::UnorderedList;
        case MD_BLOCK_OL:
            return Block::OrderedList;
        case MD_BLOCK_LI:
            return Block::ListItem;
        case MD_BLOCK_HR:
            return Block::HorizontalRule;
        case MD_BLOCK_H:
            return Block::Heading;
        case MD_BLOCK_CODE:
            return Block::CodeBlock;
        case MD_BLOCK_HTML:
            return Block::HtmlBlock;
        case MD_BLOCK_P:
            return Block::Paragraph;
        case MD_BLOCK_TABLE:
            return Block::Table;
        case MD_BLOCK_THEAD:
            return Block::TableHead;
        case MD_BLOCK_TBODY:
            return Block::TableBody;
        case MD_BLOCK_TR:
            return Block::TableRow;
        case MD_BLOCK_TH:
            return Block::TableHeaderCell;
        case MD_BLOCK_TD:
            return Block::TableCell;
        }
        return Block::Paragraph;
    }

    static Span ToSpan(MD_SPANTYPE type)
    {
        switch (type) {
        case MD_SPAN_EM:
            return Span::Emphasis;
        case MD_SPAN_STRONG:
            return Span::Strong;
        case MD_SPAN_A:
            return Span::Link;
        case MD_SPAN_IMG:
            return Span::Image;
        case MD_SPAN_CODE:
            return Span::Code;
        case MD_SPAN_DEL:
            return Span::Strikethrough;
        case MD_SPAN_LATEXMATH:
            return Span::LatexMath;
        case MD_SPAN_LATEXMATH_DISPLAY:
            return Span::LatexMathDisplay;
        case MD_SPAN_WIKILINK:
            return Span::WikiLink;
        case MD_SPAN_U:
            return Span::Underline;
        }
        return Span::Emphasis;
    }

    static Align ToAlign(MD_ALIGN align)
    {
        switch (align) {
        case MD_ALIGN_LEFT:
            return Align::Left;
        case MD_ALIGN_CENTER:
            return Align::Center;
        case MD_ALIGN_RIGHT:
            return Align::Right;
        case MD_ALIGN_DEFAULT:
            break;
        }
        return Align::Default;
    }

    static BlockInfo ToBlockInfo(MD_BLOCKTYPE type, void* detail)
    {
        BlockInfo info;
        if (detail == nullptr) {
            return info;
        }
        switch (type) {
        case MD_BLOCK_UL: {
            const auto* d = static_cast<const MD_BLOCK_UL_DETAIL*>(detail);
            info.tight = d->is_tight != 0;
            break;
        }
        case MD_BLOCK_OL: {
            const auto* d = static_cast<const MD_BLOCK_OL_DETAIL*>(detail);
            info.tight = d->is_tight != 0;
            info.start = d->start;
            break;
        }
        case MD_BLOCK_LI: {
            const auto* d = static_cast<const MD_BLOCK_LI_DETAIL*>(detail);
            info.isTask = d->is_task != 0;
            info.taskChecked = info.isTask && (d->task_mark == 'x' || d->task_mark == 'X');
            break;
        }
        case MD_BLOCK_H: {
            const auto* d = static_cast<const MD_BLOCK_H_DETAIL*>(detail);
            info.level = d->level;
            break;
        }
        case MD_BLOCK_CODE: {
            const auto* d = static_cast<const MD_BLOCK_CODE_DETAIL*>(detail);
            info.lang = ToString(d->lang);
            info.info = ToString(d->info);
            info.fenced = d->fence_char != 0;
            break;
        }
        case MD_BLOCK_TABLE: {
            const auto* d = static_cast<const MD_BLOCK_TABLE_DETAIL*>(detail);
            info.columns = d->col_count;
            break;
        }
        case MD_BLOCK_TH:
        case MD_BLOCK_TD: {
            const auto* d = static_cast<const MD_BLOCK_TD_DETAIL*>(detail);
            info.align = ToAlign(d->align);
            break;
        }
        default:
            break;
        }
        return info;
    }

    static SpanInfo ToSpanInfo(MD_SPANTYPE type, void* detail)
    {
        SpanInfo info;
        if (detail == nullptr) {
            return info;
        }
        switch (type) {
        case MD_SPAN_A: {
            const auto* d = static_cast<const MD_SPAN_A_DETAIL*>(detail);
            info.url = ToString(d->href);
            info.title = ToString(d->title);
            info.isAutolink = d->is_autolink != 0;
            break;
        }
        case MD_SPAN_IMG: {
            const auto* d = static_cast<const MD_SPAN_IMG_DETAIL*>(detail);
            info.url = ToString(d->src);
            info.title = ToString(d->title);
            break;
        }
        case MD_SPAN_WIKILINK: {
            const auto* d = static_cast<const MD_SPAN_WIKILINK_DETAIL*>(detail);
            info.url = ToString(d->target);
            break;
        }
        default:
            break;
        }
        return info;
    }

    /// Run `action`. md4c is a C library, so no exception may go through it: an exception stops the parsing.
    /// Returns what the md4c callbacks must return: non-zero stops the parsing
    template <typename Action>
    static int Run(void* userdata, Action&& action)
    {
        auto* self = static_cast<clMarkdownParser*>(userdata);
        if (self->m_aborted) {
            return 1;
        }
        try {
            action(*self);
        } catch (...) {
            self->m_aborted = true;
        }
        return self->m_aborted ? 1 : 0;
    }

    static int EnterBlock(MD_BLOCKTYPE type, void* detail, void* userdata)
    {
        return Run(userdata, [&](clMarkdownParser& self) { self.OnEnterBlock(ToBlock(type), ToBlockInfo(type, detail)); });
    }

    static int LeaveBlock(MD_BLOCKTYPE type, void* detail, void* userdata)
    {
        return Run(userdata, [&](clMarkdownParser& self) { self.OnLeaveBlock(ToBlock(type), ToBlockInfo(type, detail)); });
    }

    static int EnterSpan(MD_SPANTYPE type, void* detail, void* userdata)
    {
        return Run(userdata, [&](clMarkdownParser& self) { self.OnEnterSpan(ToSpan(type), ToSpanInfo(type, detail)); });
    }

    static int LeaveSpan(MD_SPANTYPE type, void* detail, void* userdata)
    {
        return Run(userdata, [&](clMarkdownParser& self) { self.OnLeaveSpan(ToSpan(type), ToSpanInfo(type, detail)); });
    }

    static int Text(MD_TEXTTYPE type, const MD_CHAR* text, MD_SIZE size, void* userdata)
    {
        return Run(userdata, [&](clMarkdownParser& self) {
            switch (type) {
            case MD_TEXT_NORMAL:
                self.OnText(TextType::Normal, ToString(text, size));
                break;
            case MD_TEXT_NULLCHAR:
                // CommonMark: a NULL character is replaced with U+FFFD
                self.OnText(TextType::Normal, FromCodepoint(kReplacementChar));
                break;
            case MD_TEXT_BR:
                self.OnText(TextType::Break, wxEmptyString);
                break;
            case MD_TEXT_SOFTBR:
                self.OnText(TextType::SoftBreak, wxEmptyString);
                break;
            case MD_TEXT_ENTITY:
                self.OnText(TextType::Normal, DecodeEntity(text, size));
                break;
            case MD_TEXT_CODE:
                self.OnText(TextType::Code, ToString(text, size));
                break;
            case MD_TEXT_HTML:
                self.OnText(TextType::Html, ToString(text, size));
                break;
            case MD_TEXT_LATEXMATH:
                self.OnText(TextType::LatexMath, ToString(text, size));
                break;
            }
        });
    }

    static unsigned ToMd4cFlags(unsigned flags)
    {
        struct Mapping {
            unsigned ours;
            unsigned md4c;
        };
        static constexpr Mapping kMapping[] = {
            {kTables, MD_FLAG_TABLES},
            {kStrikethrough, MD_FLAG_STRIKETHROUGH},
            {kTaskLists, MD_FLAG_TASKLISTS},
            {kPermissiveAutolinks, MD_FLAG_PERMISSIVEAUTOLINKS},
            {kUnderline, MD_FLAG_UNDERLINE},
            {kLatexMath, MD_FLAG_LATEXMATHSPANS},
            {kWikiLinks, MD_FLAG_WIKILINKS},
            {kNoHtml, MD_FLAG_NOHTML},
            {kHardSoftBreaks, MD_FLAG_HARD_SOFT_BREAKS},
        };
        unsigned result = 0;
        for (const auto& mapping : kMapping) {
            if ((flags & mapping.ours) == mapping.ours) {
                result |= mapping.md4c;
            }
        }
        return result;
    }
};

bool clMarkdownParser::Parse(const wxString& markdown)
{
    m_aborted = false;

    const wxScopedCharBuffer utf8 = markdown.utf8_str();
    // The parentheses stop a `max` macro (windows.h) from being expanded
    if (utf8.length() > (std::numeric_limits<MD_SIZE>::max)()) {
        return false;
    }

    MD_PARSER parser{};
    parser.flags = Impl::ToMd4cFlags(m_flags);
    parser.enter_block = &Impl::EnterBlock;
    parser.leave_block = &Impl::LeaveBlock;
    parser.enter_span = &Impl::EnterSpan;
    parser.leave_span = &Impl::LeaveSpan;
    parser.text = &Impl::Text;
    return md_parse(utf8.data(), static_cast<MD_SIZE>(utf8.length()), &parser, this) == 0 && !m_aborted;
}

void clMarkdownParser::OnEnterBlock(Block block, const BlockInfo& info)
{
    wxUnusedVar(block);
    wxUnusedVar(info);
}

void clMarkdownParser::OnLeaveBlock(Block block, const BlockInfo& info)
{
    wxUnusedVar(block);
    wxUnusedVar(info);
}

void clMarkdownParser::OnEnterSpan(Span span, const SpanInfo& info)
{
    wxUnusedVar(span);
    wxUnusedVar(info);
}

void clMarkdownParser::OnLeaveSpan(Span span, const SpanInfo& info)
{
    wxUnusedVar(span);
    wxUnusedVar(info);
}

void clMarkdownParser::OnText(TextType type, const wxString& text)
{
    wxUnusedVar(type);
    wxUnusedVar(text);
}
