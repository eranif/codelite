#include "FindResultsView.hpp"

#include "ColoursAndFontsManager.h"
#include "clSystemSettings.h"
#include "lexer_configuration.h"

#include <algorithm>
#include <map>
#include <set>
#include <wx/dc.h>
#include <wx/dcclient.h>
#include <wx/stc/stc.h>
#include <wx/translation.h>

namespace
{
constexpr int kPadding = 4;
// Lines longer than this are clipped around the match, a minified file may have lines of hundreds of KBs
constexpr size_t kMaxLineLength = 1000;
constexpr size_t kContextBeforeMatch = 100;
constexpr size_t kTabSize = 4;

/// What is displayed for a match: "<line number>: <text>", with a range of the text highlighted
struct MatchText {
    wxString prefix;
    wxString text;
    size_t highlightStart{0};
    size_t highlightLength{0};
};

MatchText BuildMatchText(const SearchResult& result)
{
    MatchText mt;
    mt.prefix = wxString::Format(" %5d: ", result.GetLineNumber());

    wxString pattern = result.GetPattern();
    pattern.Replace("\r", "");
    pattern.Replace("\n", "");

    const size_t matchStart = static_cast<size_t>(std::max(0, result.GetColumnInChars()));
    const size_t matchEnd = matchStart + static_cast<size_t>(std::max(0, result.GetLenInChars()));

    // Replace the tabs with spaces and keep track of where the match goes
    wxString text;
    size_t start = wxString::npos;
    size_t end = wxString::npos;
    for (size_t i = 0; i < pattern.length(); ++i) {
        if (i == matchStart) {
            start = text.length();
        }
        if (i == matchEnd) {
            end = text.length();
        }
        if (pattern[i] == '\t') {
            text.Append(' ', kTabSize);
        } else {
            text.Append(pattern[i]);
        }
    }
    if (start == wxString::npos) {
        start = text.length();
    }
    if (end == wxString::npos) {
        end = text.length();
    }

    // Clip long lines, keep the match visible
    if (text.length() > kMaxLineLength) {
        const size_t from = start > kContextBeforeMatch ? start - kContextBeforeMatch : 0;
        wxString clipped = text.Mid(from, kMaxLineLength);
        size_t shift = 0;
        if (from > 0) {
            clipped.Prepend("...");
            shift = 3;
        }
        if (from + kMaxLineLength < text.length()) {
            clipped.Append("...");
        }
        start = start - from + shift;
        end = end - from + shift;
        text = clipped;
    }

    mt.text = text;
    mt.highlightStart = std::min(start, text.length());
    mt.highlightLength = std::min(end, text.length()) - mt.highlightStart;
    return mt;
}

/// A file is displayed as "<path> (<number of matches>)"
wxString BuildFileText(const FindResultsNode& node)
{
    return wxString::Format("%s (%zu)", node.result.GetFileName(), node.children.size());
}
} // namespace

// ---------------------------------------------------------------------------
// FindResultsModel
// ---------------------------------------------------------------------------

wxString FindResultsModel::GetColumnType(unsigned int col) const { return col == kColCheck ? "bool" : "void*"; }

void FindResultsModel::GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const
{
    const FindResultsNode* node = ToNode(item);
    if (node == nullptr) {
        return;
    }

    if (col == kColCheck) {
        if (node->IsFile()) {
            // A file is checked if all its matches are checked
            const bool all_checked =
                !node->children.empty() &&
                std::all_of(node->children.begin(), node->children.end(), [](const auto& c) { return c->checked; });
            variant = all_checked;
        } else {
            variant = node->checked;
        }
    } else {
        variant = static_cast<void*>(const_cast<FindResultsNode*>(node));
    }
}

bool FindResultsModel::SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col)
{
    FindResultsNode* node = ToNode(item);
    if (node == nullptr || col != kColCheck) {
        return false;
    }

    const bool checked = variant.GetBool();
    if (node->IsFile()) {
        wxDataViewItemArray changed;
        for (auto& child : node->children) {
            child->checked = checked;
            changed.Add(ToItem(child.get()));
        }
        ItemsChanged(changed);
        ItemChanged(item);
    } else {
        node->checked = checked;
        ItemChanged(item);
        if (node->parent != nullptr) {
            ItemChanged(ToItem(node->parent));
        }
    }
    return true;
}

wxDataViewItem FindResultsModel::GetParent(const wxDataViewItem& item) const
{
    const FindResultsNode* node = ToNode(item);
    if (node == nullptr || node->parent == nullptr) {
        return wxDataViewItem();
    }
    return ToItem(node->parent);
}

bool FindResultsModel::IsContainer(const wxDataViewItem& item) const
{
    // The (invisible) root is a container
    const FindResultsNode* node = ToNode(item);
    return node == nullptr || node->IsFile();
}

unsigned int FindResultsModel::GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const
{
    const FindResultsNode* node = ToNode(item);
    if (node == nullptr) {
        for (const auto& file : m_files) {
            children.Add(ToItem(file.get()));
        }
        return m_files.size();
    }

    for (const auto& child : node->children) {
        children.Add(ToItem(child.get()));
    }
    return node->children.size();
}

void FindResultsModel::Clear()
{
    m_files.clear();
    m_matchCount = 0;
    Cleared();
}

void FindResultsModel::AddResults(const SearchResultList& results)
{
    wxDataViewItemArray newFiles;
    std::vector<FindResultsNode*> touchedFiles; // in the order they were first touched
    std::map<FindResultsNode*, wxDataViewItemArray> newMatches;

    for (const auto& result : results) {
        FindResultsNode* file = m_files.empty() ? nullptr : m_files.back().get();
        if (file == nullptr || file->result.GetFileName() != result.GetFileName()) {
            auto newFile = std::make_unique<FindResultsNode>(FindResultsNode::Kind::kFile);
            newFile->result.SetFileName(result.GetFileName());
            file = newFile.get();
            m_files.push_back(std::move(newFile));
            newFiles.Add(ToItem(file));
        }

        auto match = std::make_unique<FindResultsNode>(FindResultsNode::Kind::kMatch);
        match->parent = file;
        match->result = result;
        newMatches[file].Add(ToItem(match.get()));
        file->children.push_back(std::move(match));
        ++m_matchCount;

        // The matches of a file are always added together, so it is enough to compare with the last one
        if (touchedFiles.empty() || touchedFiles.back() != file) {
            touchedFiles.push_back(file);
        }
    }

    // The new files first, then their matches
    if (!newFiles.IsEmpty()) {
        ItemsAdded(wxDataViewItem(), newFiles);
    }
    for (FindResultsNode* file : touchedFiles) {
        ItemsAdded(ToItem(file), newMatches[file]);
        ItemChanged(ToItem(file)); // the number of matches
    }
}

void FindResultsModel::RemoveMatches(const std::vector<FindResultsNode*>& nodes)
{
    // Group the matches by their file
    std::map<FindResultsNode*, std::set<const FindResultsNode*>> toRemove;
    for (FindResultsNode* node : nodes) {
        if (node != nullptr && node->IsMatch() && node->parent != nullptr) {
            toRemove[node->parent].insert(node);
        }
    }

    std::vector<FindResultsNode*> emptyFiles;
    for (auto& [file, matches] : toRemove) {
        wxDataViewItemArray items;
        for (const FindResultsNode* match : matches) {
            items.Add(ToItem(match));
        }

        // Notify the view first: it must not access the nodes after they are deleted
        ItemsDeleted(ToItem(file), items);
        auto& children = file->children;
        children.erase(std::remove_if(children.begin(),
                                      children.end(),
                                      [&matches](const auto& c) { return matches.count(c.get()) != 0; }),
                       children.end());
        m_matchCount -= std::min(m_matchCount, matches.size());

        if (children.empty()) {
            emptyFiles.push_back(file);
        } else {
            ItemChanged(ToItem(file)); // the number of matches
        }
    }

    for (FindResultsNode* file : emptyFiles) {
        ItemDeleted(wxDataViewItem(), ToItem(file));
        m_files.erase(std::remove_if(m_files.begin(), m_files.end(), [file](const auto& f) { return f.get() == file; }),
                      m_files.end());
    }
}

void FindResultsModel::SetAllChecked(bool checked)
{
    for (const auto& file : m_files) {
        wxDataViewItemArray changed;
        for (const auto& match : file->children) {
            match->checked = checked;
            changed.Add(ToItem(match.get()));
        }
        ItemsChanged(changed);
        ItemChanged(ToItem(file.get()));
    }
}

void FindResultsModel::NodeChanged(const FindResultsNode* node)
{
    if (node != nullptr) {
        ItemChanged(ToItem(node));
    }
}

std::vector<FindResultsNode*> FindResultsModel::GetFiles() const
{
    std::vector<FindResultsNode*> files;
    files.reserve(m_files.size());
    for (const auto& file : m_files) {
        files.push_back(file.get());
    }
    return files;
}

std::vector<FindResultsNode*> FindResultsModel::GetMatches() const
{
    std::vector<FindResultsNode*> matches;
    matches.reserve(m_matchCount);
    for (const auto& file : m_files) {
        for (const auto& match : file->children) {
            matches.push_back(match.get());
        }
    }
    return matches;
}

// ---------------------------------------------------------------------------
// FindResultsRenderer
// ---------------------------------------------------------------------------

FindResultsRenderer::FindResultsRenderer()
    : wxDataViewCustomRenderer("void*", wxDATAVIEW_CELL_INERT, wxALIGN_CENTER_VERTICAL)
{
    // The defaults, until FindResultsView::ApplyTheme() sets the colours of the theme
    m_colours.text = clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    m_colours.lineNumber = clSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT);
    m_colours.file = m_colours.text;
    m_colours.matchBackground = wxColour("GOLD");
    m_colours.matchText = *wxBLACK;
    m_colours.failed = wxColour("RED");
}

bool FindResultsRenderer::SetValue(const wxVariant& value)
{
    m_node = static_cast<const FindResultsNode*>(value.GetVoidPtr());
    return true;
}

bool FindResultsRenderer::GetValue(wxVariant& value) const
{
    value = static_cast<void*>(const_cast<FindResultsNode*>(m_node));
    return true;
}

wxSize FindResultsRenderer::GetSize() const
{
    wxWindow* view = GetView();
    if (view == nullptr) {
        return wxSize(wxDVC_DEFAULT_RENDERER_SIZE, wxDVC_DEFAULT_RENDERER_SIZE);
    }

    if (m_node == nullptr) {
        const wxSize size = view->GetTextExtent("Tp");
        return wxSize(size.x + 2 * kPadding, size.y + kPadding);
    }

    int width = 0;
    int height = 0;
    if (m_node->IsFile()) {
        view->GetTextExtent(BuildFileText(*m_node), &width, &height);
    } else {
        const MatchText mt = BuildMatchText(m_node->result);
        view->GetTextExtent(mt.prefix + mt.text, &width, &height);
    }
    return wxSize(width + 2 * kPadding, height + kPadding);
}

bool FindResultsRenderer::Render(wxRect rect, wxDC* dc, int state)
{
    if (m_node == nullptr) {
        return false;
    }

    const bool selected = (state & wxDATAVIEW_CELL_SELECTED) != 0;
    // A selected row is drawn with the selection colour
    wxColour textColour = selected ? clSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT) : m_colours.text;
    wxColour dimmedColour = selected ? textColour : m_colours.lineNumber;
    wxColour fileColour = selected ? textColour : m_colours.file;
    if (!selected && m_node->state == FindResultsNode::State::kFailed) {
        textColour = m_colours.failed;
    }

    rect.Deflate(kPadding, 0);
    int x = rect.x;

    // Draw `text` at `x`, vertically centered. Advance `x`
    auto draw = [&](const wxString& text) {
        if (text.empty()) {
            return;
        }
        const wxSize size = dc->GetTextExtent(text);
        dc->DrawText(text, x, rect.y + (rect.height - size.y) / 2);
        x += size.x;
    };

    if (m_node->IsFile()) {
        dc->SetTextForeground(fileColour);
        draw(m_node->result.GetFileName());
        dc->SetTextForeground(dimmedColour);
        draw(wxString::Format(" (%zu)", m_node->children.size()));
        return true;
    }

    const MatchText mt = BuildMatchText(m_node->result);
    dc->SetTextForeground(dimmedColour);
    draw(mt.prefix);

    const wxString before = mt.text.Left(mt.highlightStart);
    const wxString match = mt.text.Mid(mt.highlightStart, mt.highlightLength);
    const wxString after = mt.text.Mid(mt.highlightStart + mt.highlightLength);

    dc->SetTextForeground(textColour);
    draw(before);

    if (!match.empty()) {
        const wxSize size = dc->GetTextExtent(match);
        wxRect matchRect(x, rect.y + 1, size.x, rect.height - 2);
        dc->SetPen(*wxTRANSPARENT_PEN);
        dc->SetBrush(wxBrush(m_colours.matchBackground));
        dc->DrawRoundedRectangle(matchRect, 2.0);
        dc->SetTextForeground(m_colours.matchText);
        draw(match);
        dc->SetTextForeground(textColour);
    }
    draw(after);
    return true;
}

// ---------------------------------------------------------------------------
// FindResultsView
// ---------------------------------------------------------------------------

FindResultsView::FindResultsView(wxWindow* parent, bool checkable)
    : m_model(new FindResultsModel())
{
    // wxDV_ROW_LINES is required for the alternate row colour
    Create(parent,
           wxID_ANY,
           wxDefaultPosition,
           wxDefaultSize,
           wxDV_NO_HEADER | wxDV_MULTIPLE | wxDV_ROW_LINES | wxBORDER_NONE);
    AssociateModel(m_model.get());

    if (checkable) {
        auto* toggle = new wxDataViewColumn(wxString(),
                                            new wxDataViewToggleRenderer("bool", wxDATAVIEW_CELL_ACTIVATABLE),
                                            FindResultsModel::kColCheck,
                                            FromDIP(28),
                                            wxALIGN_CENTER,
                                            0);
        AppendColumn(toggle);
    }

    m_renderer = new FindResultsRenderer();
    auto* column = new wxDataViewColumn(_("Results"),
                                        m_renderer,
                                        FindResultsModel::kColText,
                                        wxCOL_WIDTH_AUTOSIZE,
                                        wxALIGN_LEFT,
                                        wxDATAVIEW_COL_RESIZABLE);
    AppendColumn(column);
    SetExpanderColumn(column);

    // A hand cursor over the results. The control draws in its own child window, so set it on the children as well
    const wxCursor handCursor(wxCURSOR_HAND);
    SetCursor(handCursor);
    for (wxWindow* child : GetChildren()) {
        child->SetCursor(handCursor);
    }
    ApplyTheme();
}

FindResultsView::~FindResultsView() = default;

void FindResultsView::ApplyTheme()
{
    // The same lexer (and the same styles) that clFindResultsStyler uses
    LexerConf::Ptr_t lexer = ColoursAndFontsManager::Get().GetLexer("c++");
    if (!lexer) {
        lexer = ColoursAndFontsManager::Get().GetLexer("text");
    }
    if (!lexer) {
        return;
    }

    const StyleProperty defaultStyle = lexer->GetProperty(0);
    const StyleProperty lineNumberStyle = lexer->GetProperty(33); // 33 is the style of the line numbers
    const StyleProperty identifierStyle = lexer->GetProperty(wxSTC_C_IDENTIFIER);
    const StyleProperty wordStyle = lexer->GetProperty(wxSTC_C_WORD);

    // The row colour, and a slightly different colour for the alternate rows
    const bool is_dark = lexer->IsDark();
    const wxColour bg = defaultStyle.GetBgColour();
    SetBackgroundColour(bg);
    SetForegroundColour(defaultStyle.GetFgColour());
    SetAlternateRowColour(bg.ChangeLightness(is_dark ? 104 : 97));

    // A slightly smaller font than the editor's, to fit more rows in the view
    constexpr double kMinFontSize = 6.0;
    wxFont font = lexer->GetFontForStyle(0, this);
    font.SetFractionalPointSize(std::max(kMinFontSize, font.GetFractionalPointSize() - FromDIP(2)));
    SetFont(font);

    FindResultsColours colours;
    colours.text = identifierStyle.GetFgColour();
    colours.lineNumber = lineNumberStyle.GetFgColour();
    colours.file = wordStyle.GetFgColour();
    // The colour of the indicator that marked the matched text in the old (editor based) view
    colours.matchBackground = is_dark ? wxColour("#FFD700") : wxColour("#FF9966");
    colours.matchText = *wxBLACK;
    colours.failed = is_dark ? wxColour("#FF6B6B") : wxColour("RED");
    m_renderer->SetColours(colours);
    Refresh();
}

void FindResultsView::ExpandAllFiles()
{
    for (const FindResultsNode* file : m_model->GetFiles()) {
        Expand(FindResultsModel::ToItem(file));
    }
}

void FindResultsView::CollapseAllFiles()
{
    for (const FindResultsNode* file : m_model->GetFiles()) {
        Collapse(FindResultsModel::ToItem(file));
    }
}

void FindResultsView::ToggleFolding()
{
    // Collapse all the files, or if they are all collapsed, expand them
    for (const FindResultsNode* file : m_model->GetFiles()) {
        if (IsExpanded(FindResultsModel::ToItem(file))) {
            CollapseAllFiles();
            return;
        }
    }
    ExpandAllFiles();
}

void FindResultsView::ExpandFirstFileOnly()
{
    CollapseAllFiles();
    auto files = m_model->GetFiles();
    if (!files.empty()) {
        Expand(FindResultsModel::ToItem(files.front()));
    }
}

void FindResultsView::SelectNode(const FindResultsNode* node)
{
    if (node == nullptr) {
        return;
    }
    const wxDataViewItem item = FindResultsModel::ToItem(node);
    ExpandAncestors(item);
    UnselectAll();
    Select(item);
    SetCurrentItem(item);
    EnsureVisible(item);
}

wxString FindResultsView::GetNodeText(const FindResultsNode* node)
{
    if (node == nullptr) {
        return wxEmptyString;
    }
    if (node->IsFile()) {
        return node->result.GetFileName();
    }
    wxString text = node->result.GetPattern();
    text.Replace("\r", "");
    text.Replace("\n", "");
    return wxString::Format("%d: %s", node->result.GetLineNumber(), text);
}

wxString FindResultsView::GetSelectedText() const
{
    wxDataViewItemArray selections;
    GetSelections(selections);

    wxString text;
    for (const auto& item : selections) {
        text << GetNodeText(FindResultsModel::ToNode(item)) << "\n";
    }
    return text;
}
