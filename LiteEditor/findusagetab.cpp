//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : findusagetab.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "findusagetab.h"

#include "ColoursAndFontsManager.h"
#include "LineSyntaxStyler.hpp"
#include "aui/clAuiToolBarArt.h"
#include "clStrings.h"
#include "clSystemSettings.h"
#include "clWorkspaceManager.h"
#include "cl_config.h"
#include "cl_editor.h"
#include "codelite_events.h"
#include "drawingutils.h"
#include "event_notifier.h"
#include "file_logger.h"
#include "globals.h"
#include "lexer_configuration.h"
#include "macros.h"
#include "plugin.h"
#include "wxTerminalCtrl/clBuiltinTerminalPane.hpp"

#include <algorithm>
#include <map>
#include <wx/dc.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/settings.h>
#include <wx/splitter.h>
#include <wx/srchctrl.h>
#include <wx/stc/stc.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>

#ifdef __WXOSX__
#include <objc/message.h>
#include <objc/runtime.h>
#endif

/// A match: a row of the matches view
struct FindUsageEntry {
    const LSP::Location* location = nullptr;
    int lineNumber = 0;        // the line of the match, starting at 1
    wxString text;             // the matched line, the tabs replaced by spaces (empty if it could not be read)
    size_t highlightStart = 0; // the range of `text` to highlight
    size_t highlightLength = 0;
    std::vector<LineSyntaxStyler::Part> parts; // the syntax highlight of `text`
    bool visible = true;                       // does it pass the filter?
};

/// A file: a row of the files view, and its matches
struct FindUsageFile {
    wxString path; // relative to the workspace folder when it is inside it
    std::vector<FindUsageEntry*> entries;
};

namespace
{
constexpr int kPadding = 1;
constexpr size_t kTabSize = 4;
// The toolbar buttons (the name is turned into an ID with XRCID)
constexpr const char* kToolClear = "find_usage_clear";
constexpr const char* kToolHideBuildFolders = "find_usage_hide_build_folders";
// The position of the sash between the files and the matches
const wxString kConfigSashPosition = "FindUsageTab/SashPosition";
constexpr int kDefaultSashPosition = 300;

/// Replace the tabs of `line` with spaces. The range [`start`, `start` + `length`) of `line` is moved to the same
/// characters of the result: [`highlightStart`, `highlightStart` + `highlightLength`)
wxString PrepareLine(const wxString& line, size_t start, size_t length, size_t& highlightStart, size_t& highlightLength)
{
    const size_t end = start + length;
    size_t newStart = wxString::npos;
    size_t newEnd = wxString::npos;
    wxString text;
    for (size_t i = 0; i < line.length(); ++i) {
        if (i == start) {
            newStart = text.length();
        }
        if (i == end) {
            newEnd = text.length();
        }
        if (line[i] == '\t') {
            text.Append(' ', kTabSize);
        } else {
            text.Append(line[i]);
        }
    }
    if (newStart == wxString::npos) {
        newStart = text.length();
    }
    if (newEnd == wxString::npos) {
        newEnd = text.length();
    }
    highlightStart = newStart;
    highlightLength = newEnd - newStart;
    return text;
}

/// `path` relative to the folder of the workspace, if it is inside it. The paths are compared as strings, so this
/// works for the paths of a remote workspace too
wxString MakeRelativeToWorkspace(const wxString& path)
{
    if (!clWorkspaceManager::Get().IsWorkspaceOpened()) {
        return path;
    }
    wxString dir = clWorkspaceManager::Get().GetWorkspace()->GetDir();
    if (dir.empty()) {
        return path;
    }
    if (!dir.EndsWith("/") && !dir.EndsWith("\\")) {
        dir << wxFileName::GetPathSeparator();
    }
    wxString relative;
    if (path.StartsWith(dir, &relative)) {
        return relative;
    }
    // The other separator (for example, a workspace folder with '\' and a path with '/')
    wxString other = dir;
    other.Last() = other.Last() == '/' ? '\\' : '/';
    if (path.StartsWith(other, &relative)) {
        return relative;
    }
    return path;
}

/// Is `path` (relative to the workspace when it is inside it) in a build folder, a folder whose name starts with
/// ".build" (for example ".build-release" or ".build-debug")?
bool IsInBuildFolder(const wxString& path)
{
    wxString dirs = path;
    dirs.Replace("\\", "/");
    wxArrayString parts = ::wxStringTokenize(dirs.BeforeLast('/'), "/", wxTOKEN_STRTOK);
    for (const wxString& part : parts) {
        if (part.StartsWith(".build")) {
            return true;
        }
    }
    return false;
}

/// The path of a local copy of `path`: `path` itself, or the file downloaded from the remote machine. Empty on error
wxString GetLocalFile(const wxString& path)
{
    if (wxFileName::Exists(path)) {
        return path;
    }
    // not a local file, request for file download
    clCommandEvent event_download{wxEVT_DOWNLOAD_FILE};
    event_download.SetFileName(path);
    clDEBUG() << "Sending event wxEVT_DOWNLOAD_FILE" << endl;
    if (!EventNotifier::Get()->ProcessEvent(event_download)) {
        return wxEmptyString;
    }
    return event_download.GetFileName();
}

/// The DC that a native control (macOS) gives to a custom renderer has the default font, not the font of the control:
/// use the font of the `view` of the renderer, as GetSize() does
void UseViewFont(const wxWindow* view, wxDC* dc)
{
    if (view != nullptr) {
        dc->SetFont(view->GetFont());
    }
}
/// On macOS 11 and later, a table view with only one column gets the "inset" style by default: it has a padding around
/// its rows, so its first row is lower than the first row of a table with two columns. Use the "full width" style, as
/// the other tables. wxWidgets has no API for this, so the method is called with the Objective-C runtime
void UseFullWidthStyle(wxDataViewCtrl* ctrl)
{
#ifdef __WXOSX__
    // The handle is the scroll view, the table is its document view
    id scroll_view = reinterpret_cast<id>(ctrl->GetHandle());
    if (scroll_view == nil) {
        return;
    }
    id table_view = reinterpret_cast<id (*)(id, SEL)>(objc_msgSend)(scroll_view, sel_registerName("documentView"));
    SEL set_style = sel_registerName("setStyle:");
    if (table_view == nil || !class_respondsToSelector(object_getClass(table_view), set_style)) {
        return;
    }
    constexpr long kNSTableViewStyleFullWidth = 1;
    reinterpret_cast<void (*)(id, SEL, long)>(objc_msgSend)(table_view, set_style, kNSTableViewStyleFullWidth);
#else
    wxUnusedVar(ctrl);
#endif
}
} // namespace

/// Draws a matched line, with the matched range highlighted
class FindUsageRenderer : public wxDataViewCustomRenderer
{
public:
    struct Colours {
        wxColour match;   // a matched line (its plain text)
        wxColour keyword; // the syntax highlight of the line
        wxColour function;
        wxColour className;
        wxColour string;
        wxColour number;
        wxColour comment;
        wxColour matchBackground; // the rounded rectangle around the matched range of the line
        wxColour matchText;       // the text of the matched range, it contrasts with matchBackground
        wxColour selection;       // the dotted rectangle around the selected row
    };

    FindUsageRenderer()
        : wxDataViewCustomRenderer("void*", wxDATAVIEW_CELL_INERT, wxALIGN_CENTER_VERTICAL)
    {
        m_colours.match = clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
        m_colours.keyword = m_colours.function = m_colours.className = m_colours.string = m_colours.number =
            m_colours.comment = m_colours.match;
        m_colours.matchBackground = wxColour("GOLD");
        m_colours.matchText = *wxBLACK;
        m_colours.selection = m_colours.match;
    }

    void SetColours(const Colours& colours) { m_colours = colours; }
    /// The entry of the selected row. We do not use the state of the cell: on macOS a row is not drawn as selected
    /// when the view does not have the focus, and selecting a row moves the focus to the editor
    void SetSelectedEntry(const FindUsageEntry* entry) { m_selected = entry; }

    bool SetValue(const wxVariant& value) override
    {
        m_entry = static_cast<const FindUsageEntry*>(value.GetVoidPtr());
        return true;
    }

    bool GetValue(wxVariant& value) const override
    {
        value = static_cast<void*>(const_cast<FindUsageEntry*>(m_entry));
        return true;
    }

    wxSize GetSize() const override
    {
        wxWindow* view = GetView();
        if (view == nullptr) {
            return wxSize(wxDVC_DEFAULT_RENDERER_SIZE, wxDVC_DEFAULT_RENDERER_SIZE);
        }
        // The height of a text line, as FindUsageTextRenderer: the rows of the two views have the same height
        const int width = m_entry != nullptr ? view->GetTextExtent(m_entry->text).x : 0;
        return wxSize(width + 2 * kPadding, view->GetTextExtent("Tp").y);
    }

    bool Render(wxRect rect, wxDC* dc, int state) override
    {
        wxUnusedVar(state);
        if (m_entry == nullptr) {
            return false;
        }
        const bool selected = m_entry == m_selected;

        // The colours of the text do not change on a selected row, so the row is marked with a soft dotted rectangle
        if (selected) {
            wxRect outline = rect; // around the text, in case we cannot find the row
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
            // The control clips the cell, and gives us the box of the text, not of the row. The clipping box has the
            // height of the row: remove the clipping for a moment, to draw around the entire line, from the left edge
            // of the view to its right edge
            wxRect clip;
            const bool hasClip = dc->GetClippingBox(clip) && !clip.IsEmpty();
            if (hasClip) {
                const int viewWidth = GetView() != nullptr ? GetView()->GetClientSize().x : 0;
                outline = wxRect(0, clip.y, std::max(viewWidth, clip.GetRight() + 1), clip.height);
                dc->DestroyClippingRegion();
            }
#endif
            outline.Deflate(1);
            dc->SetPen(wxPen(m_colours.selection, 1, wxPENSTYLE_DOT));
            dc->SetBrush(*wxTRANSPARENT_BRUSH);
            dc->DrawRectangle(outline);
#ifdef wxHAS_GENERIC_DATAVIEWCTRL
            if (hasClip) {
                dc->SetClippingRegion(clip);
            }
#endif
        }

        // A long line must not be drawn outside of its cell
        wxDCClipper clipper(*dc, rect);
        UseViewFont(GetView(), dc);
        rect.Deflate(kPadding, 0);
        int x = rect.x;

        const int y_text = rect.y + (rect.height - dc->GetTextExtent("Tp").y) / 2;
        const wxString& text = m_entry->text;

        // Only the match of the selected row is highlighted, many highlights are too much
        size_t highlight_start = 0;
        size_t highlight_end = 0;
        if (selected && m_entry->highlightLength > 0) {
            highlight_start = m_entry->highlightStart;
            highlight_end = m_entry->highlightStart + m_entry->highlightLength;
            // A rounded rectangle around the match, the text is drawn over it
            const int start_x = x + dc->GetTextExtent(text.Left(highlight_start)).x;
            const int width = dc->GetTextExtent(text.Mid(highlight_start, m_entry->highlightLength)).x;
            dc->SetPen(*wxTRANSPARENT_PEN);
            dc->SetBrush(wxBrush(m_colours.matchBackground));
            dc->DrawRoundedRectangle(wxRect(start_x - 1, rect.y + 1, width + 2, rect.height - 2), 3.0);
        }

        // Draw the parts of the line in their colours. A part is cut where the highlighted match starts and ends: the
        // text of the match has a colour that contrasts with its rectangle
        for (const LineSyntaxStyler::Part& part : m_entry->parts) {
            size_t pos = part.start;
            const size_t end = part.start + part.length;
            while (pos < end) {
                const bool in_match = pos >= highlight_start && pos < highlight_end;
                size_t next = end;
                if (!in_match && pos < highlight_start && highlight_start < end) {
                    next = highlight_start;
                } else if (in_match && highlight_end < end) {
                    next = highlight_end;
                }
                const wxString segment = text.Mid(pos, next - pos);
                dc->SetTextForeground(in_match ? m_colours.matchText : GetStyleColour(part.style));
                dc->DrawText(segment, x, y_text);
                x += dc->GetTextExtent(segment).x;
                pos = next;
            }
        }
        return true;
    }

private:
    const wxColour& GetStyleColour(LineSyntaxStyler::Style style) const
    {
        switch (style) {
        case LineSyntaxStyler::Style::Keyword:
            return m_colours.keyword;
        case LineSyntaxStyler::Style::Function:
            return m_colours.function;
        case LineSyntaxStyler::Style::Class:
            return m_colours.className;
        case LineSyntaxStyler::Style::String:
            return m_colours.string;
        case LineSyntaxStyler::Style::Number:
            return m_colours.number;
        case LineSyntaxStyler::Style::Comment:
            return m_colours.comment;
        case LineSyntaxStyler::Style::Default:
            break;
        }
        return m_colours.match;
    }

    const FindUsageEntry* m_entry = nullptr;
    const FindUsageEntry* m_selected = nullptr;
    Colours m_colours;
};

/// Draws a text (a file, a line number) with the font and the colour of the matched lines, so both views look the same
class FindUsageTextRenderer : public wxDataViewCustomRenderer
{
public:
    explicit FindUsageTextRenderer(bool alignRight = false)
        : wxDataViewCustomRenderer("string", wxDATAVIEW_CELL_INERT, wxALIGN_CENTER_VERTICAL)
        , m_alignRight(alignRight)
    {
    }

    void SetColour(const wxColour& colour) { m_colour = colour; }

    bool SetValue(const wxVariant& value) override
    {
        m_text = value.GetString();
        return true;
    }

    bool GetValue(wxVariant& value) const override
    {
        value = m_text;
        return true;
    }

    wxSize GetSize() const override
    {
        wxWindow* view = GetView();
        if (view == nullptr) {
            return wxSize(wxDVC_DEFAULT_RENDERER_SIZE, wxDVC_DEFAULT_RENDERER_SIZE);
        }
        // The height of a text line, as FindUsageRenderer
        return wxSize(view->GetTextExtent(m_text).x + 2 * kPadding, view->GetTextExtent("Tp").y);
    }

    bool Render(wxRect rect, wxDC* dc, int state) override
    {
        wxUnusedVar(state);
        wxDCClipper clipper(*dc, rect);
        UseViewFont(GetView(), dc);
        rect.Deflate(kPadding, 0);
        const wxSize size = dc->GetTextExtent(m_text);
        const int x = m_alignRight ? rect.GetRight() - size.x : rect.x;
        dc->SetTextForeground(m_colour.IsOk() ? m_colour : clSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        dc->DrawText(m_text, x, rect.y + (rect.height - size.y) / 2);
        return true;
    }

private:
    wxString m_text;
    wxColour m_colour;
    bool m_alignRight = false;
};

FindUsageTab::FindUsageTab(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));
    SetSizer(new wxBoxSizer(wxVERTICAL));

    // The toolbar. The tools are created by BuildToolbar() (again when the bitmaps change), the events are bound once
    m_toolbar = new wxAuiToolBar(this);
    m_toolbar->Bind(wxEVT_MENU, &FindUsageTab::OnClearAll, this, XRCID(kToolClear));
    m_toolbar->Bind(wxEVT_UPDATE_UI, &FindUsageTab::OnClearAllUI, this, XRCID(kToolClear));
    m_toolbar->Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& event) {
            m_hideBuildFolders = event.IsChecked();
            ApplyFilter();
        },
        XRCID(kToolHideBuildFolders));
    m_toolbar->Bind(
        wxEVT_UPDATE_UI,
        [this](wxUpdateUIEvent& event) { event.Check(m_hideBuildFolders); },
        XRCID(kToolHideBuildFolders));
    GetSizer()->Add(m_toolbar, 0, wxEXPAND);
    BuildToolbar();

    // The filter: a match is shown when its file or its matched line contains the text
    m_filter = new wxSearchCtrl(this, wxID_ANY);
    m_filter->SetHint(_("Filter results. Prefix a string with a minus sign (-) to exclude matches."));
    m_filter->ShowCancelButton(true);

    m_filter->Bind(wxEVT_SEARCH, &FindUsageTab::OnFilter, this);
    m_filter->Bind(wxEVT_SEARCH_CANCEL, [this](wxCommandEvent& event) {
        wxUnusedVar(event);
        m_filter->ChangeValue(wxEmptyString);
        ApplyFilter();
    });

    GetSizer()->Add(m_filter, 0, wxEXPAND | wxALL, FromDIP(5));

    // The files on the left, the matches of the selected file on the right. The sash position is remembered
    m_splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_LIVE_UPDATE);
    m_splitter->SetMinimumPaneSize(FromDIP(50));

    m_dvFiles =
        new wxDataViewListCtrl(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES | wxBORDER_NONE);
    // Both views use our renderers, with the same font and row height, so their rows are aligned
    m_fileRenderer = new FindUsageTextRenderer();
    m_dvFiles->AppendColumn(
        new wxDataViewColumn(_("File"), m_fileRenderer, 0, wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxCOL_RESIZABLE),
        "string");

    m_dvMatches =
        new wxDataViewListCtrl(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxDV_ROW_LINES | wxBORDER_NONE);
    m_lineRenderer = new FindUsageTextRenderer(true);
    m_dvMatches->AppendColumn(
        new wxDataViewColumn(_("Line"), m_lineRenderer, 0, wxCOL_WIDTH_AUTOSIZE, wxALIGN_RIGHT, wxCOL_RESIZABLE),
        "string");
    // The matched line is drawn with our renderer: the value of the cell is the entry
    m_renderer = new FindUsageRenderer();
    m_dvMatches->AppendColumn(
        new wxDataViewColumn(_("Match"), m_renderer, 1, wxCOL_WIDTH_AUTOSIZE, wxALIGN_LEFT, wxCOL_RESIZABLE), "void*");

    UseFullWidthStyle(m_dvFiles);
    UseFullWidthStyle(m_dvMatches);

    m_savedSashPosition = clConfig::Get().Read(kConfigSashPosition, FromDIP(kDefaultSashPosition));
    m_splitter->SplitVertically(m_dvFiles, m_dvMatches, m_savedSashPosition);

    // wxSplitterWindow also sends wxEVT_SPLITTER_SASH_POS_CHANGED when it moves the sash by itself, to keep the panes
    // at their minimum size when the window is small (this happens while the layout is built at startup). Do not save
    // these positions. wxEVT_SPLITTER_SASH_POS_CHANGING is sent only while the user drags the sash, so save the
    // position only when it comes after it
    m_splitter->Bind(wxEVT_SPLITTER_SASH_POS_CHANGING, [this](wxSplitterEvent& event) {
        event.Skip();
        m_userMovingSash = true;
    });
    m_splitter->Bind(wxEVT_SPLITTER_SASH_POS_CHANGED, [this](wxSplitterEvent& event) {
        event.Skip();
        if (!m_userMovingSash) {
            return;
        }
        m_userMovingSash = false;
        m_savedSashPosition = event.GetSashPosition();
        clConfig::Get().Write(kConfigSashPosition, m_savedSashPosition);
    });

    // The window is small until the tab is shown, so wxSplitterWindow moves the sash closer to the left, and it does
    // not move it back when the window grows. Whenever the saved position fits, set it again
    m_splitter->Bind(wxEVT_SIZE, [this](wxSizeEvent& event) {
        event.Skip();
        if (!m_splitter->IsSplit()) {
            return;
        }
        const int minWidth = m_savedSashPosition + m_splitter->GetMinimumPaneSize() + m_splitter->GetSashSize();
        if (m_splitter->GetClientSize().x >= minWidth && m_splitter->GetSashPosition() != m_savedSashPosition) {
            m_splitter->SetSashPosition(m_savedSashPosition);
        }
    });

    m_dvFiles->Bind(wxEVT_SIZE, [this](wxSizeEvent& e) {
        e.Skip();
        AutoSizeColumns(m_dvFiles);
    });

    // A hand cursor over the results. The controls draw in their own child window, so set it on the children as well
    const wxCursor handCursor(wxCURSOR_HAND);
    for (wxDataViewListCtrl* ctrl : {m_dvFiles, m_dvMatches}) {
        ctrl->SetCursor(handCursor);
        for (wxWindow* child : ctrl->GetChildren()) {
            child->SetCursor(handCursor);
        }
    }

    m_dvFiles->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &FindUsageTab::OnFileSelected, this);
    m_dvMatches->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &FindUsageTab::OnItemActivated, this);
    m_dvMatches->Bind(wxEVT_DATAVIEW_SELECTION_CHANGED, &FindUsageTab::OnItemActivated, this);

    // show context menu for the controls. The data view does not send wxEVT_CONTEXT_MENU, it sends its own event (also
    // when the user clicks outside of the items)
    auto showContextMenu = [this](wxDataViewEvent& event) {
        wxMenu menu;
        menu.Append(wxID_CLEAR);
        menu.Bind(
            wxEVT_MENU,
            [this](wxCommandEvent& event) {
                wxUnusedVar(event);
                Clear();
            },
            wxID_CLEAR);
        if (auto* ctrl = dynamic_cast<wxWindow*>(event.GetEventObject())) {
            ctrl->PopupMenu(&menu);
        }
    };
    m_dvFiles->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, showContextMenu);
    m_dvMatches->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, showContextMenu);

    GetSizer()->Add(m_splitter, 1, wxEXPAND);
    GetSizer()->Layout();

    // The colours come from the terminal theme, but the tab is created inside the constructor of the output pane: the
    // terminal pane (and the frame's pointer to the output pane) do not exist yet. Update the style when they do
    CallAfter(&FindUsageTab::UpdateStyle);

    // bind events
    EventNotifier::Get()->Bind(wxEVT_SYS_COLOURS_CHANGED, &FindUsageTab::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_BUILTIN_TERMINAL_THEME_CHANGED, &FindUsageTab::OnThemeChanged, this);
    EventNotifier::Get()->Bind(wxEVT_BITMAPS_UPDATED, &FindUsageTab::OnBitmapsUpdated, this);
    EventNotifier::Get()->Bind(wxEVT_WORKSPACE_CLOSED, &FindUsageTab::OnWorkspaceClosed, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_REFERENCES, &FindUsageTab::OnReferences, this);
    EventNotifier::Get()->Bind(wxEVT_LSP_REFERENCES_INPROGRESS, &FindUsageTab::OnReferencesInProgress, this);
}

FindUsageTab::~FindUsageTab()
{
    EventNotifier::Get()->Unbind(wxEVT_SYS_COLOURS_CHANGED, &FindUsageTab::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_BUILTIN_TERMINAL_THEME_CHANGED, &FindUsageTab::OnThemeChanged, this);
    EventNotifier::Get()->Unbind(wxEVT_BITMAPS_UPDATED, &FindUsageTab::OnBitmapsUpdated, this);
    EventNotifier::Get()->Unbind(wxEVT_WORKSPACE_CLOSED, &FindUsageTab::OnWorkspaceClosed, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_REFERENCES, &FindUsageTab::OnReferences, this);
    EventNotifier::Get()->Unbind(wxEVT_LSP_REFERENCES_INPROGRESS, &FindUsageTab::OnReferencesInProgress, this);

    m_dvFiles->Unbind(wxEVT_DATAVIEW_SELECTION_CHANGED, &FindUsageTab::OnFileSelected, this);
    m_dvMatches->Unbind(wxEVT_DATAVIEW_SELECTION_CHANGED, &FindUsageTab::OnItemActivated, this);
    m_dvMatches->Unbind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &FindUsageTab::OnItemActivated, this);

    // The rows point to the files and the entries: delete them first
    m_dvFiles->DeleteAllItems();
    m_dvMatches->DeleteAllItems();
}

void FindUsageTab::Clear()
{
    // The rows point to the files and the entries, and the entries to the locations: delete them in this order
    m_renderer->SetSelectedEntry(nullptr);
    m_dvMatches->DeleteAllItems();
    m_dvFiles->DeleteAllItems();
    m_files.clear();
    m_entries.clear();
    m_locations.clear();
}

void FindUsageTab::OnClearAll(wxCommandEvent& e)
{
    wxUnusedVar(e);
    Clear();
}

void FindUsageTab::OnClearAllUI(wxUpdateUIEvent& e) { e.Enable(!m_entries.empty()); }

void FindUsageTab::InitialiseView(const std::vector<LSP::Location>& locations)
{
    Clear();
    UpdateStyle();

    m_locations = locations;
    // sort by file / location
    std::map<wxString, std::vector<const LSP::Location*>> sorted_entries;
    for (const LSP::Location& location : m_locations) {
        sorted_entries[location.GetPath()].push_back(&location);
    }

    // The lines are read now: the filter needs them
    wxBusyCursor bc;
    for (const auto& [filename, file_matches] : sorted_entries) {
        DoAddFileEntries(filename, file_matches);
    }

    // A new search: show all its results
    m_filter->ChangeValue(wxEmptyString);
    ApplyFilter();
}

void FindUsageTab::DoAddFileEntries(const wxString& filename, const std::vector<const LSP::Location*>& matches)
{
    // The file is shown relative to the workspace. Use the path from the language server: a remote file is read from
    // a local copy
    auto file = std::make_unique<FindUsageFile>();
    file->path = MakeRelativeToWorkspace(filename);

    // The syntax highlight uses the keywords and the comments of the language of the file
    LineSyntaxStyler styler;
    if (auto lexer = ColoursAndFontsManager::Get().GetLexerForFile(filename)) {
        styler = LineSyntaxStyler{lexer->GetKeyWords(0)};
        styler.SetLanguage(lexer->GetName());
    }

    wxTextFile text_buffer;
    const wxString local_file = GetLocalFile(filename);
    if (local_file.empty() || !text_buffer.Open(local_file)) {
        clWARNING() << "Find Usage: failed to read file:" << filename << endl;
    }

    for (const LSP::Location* location : matches) {
        auto entry = std::make_unique<FindUsageEntry>();
        entry->location = location;
        // The numbers of the lines of the locations start at 0
        entry->lineNumber = static_cast<int>(location->GetRange().GetStart().GetLine()) + 1;

        // in case this was a remote file, open the local copy
        if (!local_file.empty()) {
            const_cast<LSP::Location*>(location)->SetPath(local_file);
        }

        const auto& start = location->GetRange().GetStart();
        const auto& end = location->GetRange().GetEnd();
        const size_t line_number = start.GetLine();
        if (text_buffer.IsOpened() && line_number < text_buffer.GetLineCount()) {
            // the text and the highlight information (a match that spans several lines is not highlighted)
            const bool single_line = start.GetLine() == end.GetLine() && end.GetCharacter() >= start.GetCharacter();
            const size_t length = single_line ? end.GetCharacter() - start.GetCharacter() : 0;
            entry->text = PrepareLine(text_buffer.GetLine(line_number),
                                      start.GetCharacter(),
                                      length,
                                      entry->highlightStart,
                                      entry->highlightLength);
            if (!single_line) {
                entry->highlightLength = 0;
            }
        }
        entry->parts = styler.Split(entry->text);
        file->entries.push_back(entry.get());
        m_entries.push_back(std::move(entry));
    }
    m_files.push_back(std::move(file));
}

void FindUsageTab::ApplyFilter()
{
    wxString filter = m_filter->GetValue().Lower();
    bool hasFilter = !filter.empty();
    bool isExcludePattern = !filter.empty() && filter[0] == '-';
    if (isExcludePattern) {
        filter = filter.Mid(1);
        hasFilter = !filter.empty();
    }

    // Keep the selected file, if it is still shown
    const FindUsageFile* selected = GetSelectedFile();

    wxWindowUpdateLocker locker{m_dvFiles};
    m_dvFiles->DeleteAllItems();
    wxDataViewItem selected_item;
    for (const auto& file : m_files) {
        const wxString fileLower = file->path.Lower();
        const bool hidden_folder = m_hideBuildFolders && IsInBuildFolder(file->path);
        bool any_visible = false;
        for (FindUsageEntry* entry : file->entries) {
            const bool matches = fileLower.Contains(filter) || entry->text.Lower().Contains(filter);
            entry->visible = !hidden_folder && (!hasFilter || matches != isExcludePattern);
            any_visible = any_visible || entry->visible;
        }
        if (!any_visible) {
            continue;
        }

        wxVector<wxVariant> values;
        values.push_back(wxVariant(file->path));
        m_dvFiles->AppendItem(values, reinterpret_cast<wxUIntPtr>(file.get()));
        if (file.get() == selected || !selected_item.IsOk()) {
            selected_item = m_dvFiles->RowToItem(m_dvFiles->GetItemCount() - 1);
        }
    }

    if (selected_item.IsOk()) {
        m_dvFiles->Select(selected_item);
        m_dvFiles->EnsureVisible(selected_item);
    }
    ShowFileMatches(GetSelectedFile());
}

const FindUsageFile* FindUsageTab::GetSelectedFile() const
{
    const wxDataViewItem item = m_dvFiles->GetSelection();
    return item.IsOk() ? reinterpret_cast<const FindUsageFile*>(m_dvFiles->GetItemData(item)) : nullptr;
}

void FindUsageTab::ShowFileMatches(const FindUsageFile* file)
{
    wxWindowUpdateLocker locker{m_dvMatches};
    // The rows are created again, without a selection
    m_renderer->SetSelectedEntry(nullptr);
    m_dvMatches->DeleteAllItems();
    CHECK_PTR_RET(file);

    for (FindUsageEntry* entry : file->entries) {
        if (!entry->visible) {
            continue;
        }
        wxVector<wxVariant> values;
        values.push_back(wxVariant(wxString() << entry->lineNumber));
        values.push_back(wxVariant(static_cast<void*>(entry)));
        m_dvMatches->AppendItem(values, reinterpret_cast<wxUIntPtr>(entry));
    }

    if (m_dvMatches->GetItemCount()) {
        // Always have a selection.
        m_dvMatches->SelectRow(0);
        wxDataViewEvent dummy;
        DoMatchSelected(0, dummy);
    }
}

void FindUsageTab::OnFileSelected(wxDataViewEvent& event)
{
    event.Skip();
    ShowFileMatches(GetSelectedFile());
}

void FindUsageTab::OnFilter(wxCommandEvent& event)
{
    event.Skip();
    ApplyFilter();
}

void FindUsageTab::OnThemeChanged(clCommandEvent& e)
{
    e.Skip();
    UpdateStyle();
}

void FindUsageTab::OnBitmapsUpdated(clCommandEvent& e)
{
    e.Skip();
    BuildToolbar();
}

void FindUsageTab::BuildToolbar()
{
    auto* icons = clGetManager()->GetStdIcons();
    m_toolbar->Clear();
    clAuiToolBarArt::AddTool(
        m_toolbar, XRCID(kToolClear), _("Clear"), icons->LoadBitmap("clear"), _("Clear the results"));
    clAuiToolBarArt::AddTool(m_toolbar,
                             XRCID(kToolHideBuildFolders),
                             _("Hide Results From Build Folders"),
                             icons->LoadBitmap("filter"),
                             _("Hide the results from the build folders (the folders whose name starts with .build)"),
                             wxITEM_CHECK);
    // The tools are created again when the bitmaps change: keep the state of the toggle button
    m_toolbar->ToggleTool(XRCID(kToolHideBuildFolders), m_hideBuildFolders);
    clAuiToolBarArt::Finalise(m_toolbar);
    m_toolbar->Realize();
    // The new tools can have another size (a new theme has other icons): tell the sizer, or the toolbar keeps its old
    // size until the window is resized
    m_toolbar->InvalidateBestSize();
    Layout();
    m_toolbar->Refresh();
}

void FindUsageTab::UpdateStyle()
{
    // The font of the editor, with the size of the font of the system, a bit bigger
    if (auto lexer = ColoursAndFontsManager::Get().GetLexer("c++")) {
        wxFont font = lexer->GetFontForStyle(0, this);
        m_dvMatches->SetFont(font);
    }

    // The same row height in both views, so their rows are aligned
    const int rowHeight = m_dvFiles->GetTextExtent("Tp").y + FromDIP(kPadding);
    m_dvFiles->SetRowHeight(rowHeight);
    m_dvMatches->SetRowHeight(rowHeight);

    // The colours are taken from the palette of the active terminal theme. Some of them have alpha: the view is solid
    auto solid = [](const wxColour& colour) { return wxColour(colour.Red(), colour.Green(), colour.Blue()); };
    auto* terminals = clGetManager()->GetTerminalManager();
    const wxTerminalTheme theme = terminals != nullptr ? terminals->GetActiveTheme() : wxTerminalTheme::MakeDarkTheme();

    ::AdjustDataViewAlternateColour(m_dvFiles);
    ::AdjustDataViewAlternateColour(m_dvMatches);

    FindUsageRenderer::Colours colours;
    colours.match = solid(theme.fg);
    // The syntax colours: the bright colours of the palette on a dark background, the normal ones on a light background
    const bool dark = DrawingUtils::IsDark(m_dvMatches->GetBackgroundColour());
    colours.keyword = solid(dark ? theme.brightBlue : theme.blue);
    colours.function = solid(dark ? theme.brightYellow : theme.yellow);
    colours.className = solid(dark ? theme.brightMagenta : theme.magenta);
    colours.string = solid(dark ? theme.brightGreen : theme.green);
    colours.number = solid(dark ? theme.brightCyan : theme.cyan);
    colours.comment = solid(theme.brightBlack);
    colours.matchBackground = solid(theme.highlightBg);
    colours.matchText = DrawingUtils::IsDark(colours.matchBackground) ? *wxWHITE : *wxBLACK;
    colours.selection = solid(theme.brightBlack);
    m_renderer->SetColours(colours);
    m_fileRenderer->SetColour(colours.match);
    // The line numbers are dimmed
    m_lineRenderer->SetColour(solid(theme.brightBlack));
    m_dvFiles->Refresh();
    m_dvMatches->Refresh();
}

void FindUsageTab::OnWorkspaceClosed(clWorkspaceEvent& event)
{
    event.Skip();
    Clear();
}

void FindUsageTab::OnReferences(LSPEvent& event)
{
    event.Skip();
    InitialiseView(event.GetLocations());

    // ensure that this view is shown
    clGetManager()->ShowOutputPane(SHOW_USAGE);
}

void FindUsageTab::OnReferencesInProgress(LSPEvent& event)
{
    event.Skip();
    Clear();
}

void FindUsageTab::OnItemActivated(wxDataViewEvent& event)
{
    event.Skip();
    DoMatchSelected(event.GetItem(), event);
}

void FindUsageTab::DoMatchSelected(const wxDataViewItem& item, wxDataViewEvent& evt)
{
    CHECK_ITEM_RET(item);

    const auto* entry = reinterpret_cast<const FindUsageEntry*>(m_dvMatches->GetItemData(item));
    CHECK_PTR_RET(entry);

    // Keep the match of this row highlighted, also after the editor takes the focus
    m_renderer->SetSelectedEntry(entry);
    m_dvMatches->Refresh();

    // Open the file
    wxFileName fn(entry->location->GetPath());

    // prepare the "after-file-is-loaded" callback
    auto range = entry->location->GetRange();
    auto callback = [=](IEditor* editor) {
        editor->GetCtrl()->ClearSelections();
        editor->SelectRangeAfter(range);
    };

    if (fn.FileExists()) {
        // default behaviour
        CallAfter([fn, callback = std::move(callback)]() {
            clGetManager()->OpenFileAndAsyncExecute(fn.GetFullPath(), std::move(callback));
        });
        return;
    }

    // the file does not exist
    clCommandEvent open_file_event{wxEVT_OPEN_FILE};
    open_file_event.SetFileName(entry->location->GetPath());
    if (!EventNotifier::Get()->ProcessEvent(open_file_event)) {
        ::wxMessageBox(
            _("Failed to open file: ") + entry->location->GetPath(), "CodeLite", wxOK | wxOK_DEFAULT | wxICON_ERROR);
        evt.Veto();
        return;
    }

    CallAfter([fn = open_file_event.GetFileName(), callback = std::move(callback)]() {
        clGetManager()->OpenFileAndAsyncExecute(fn, std::move(callback));
    });
}