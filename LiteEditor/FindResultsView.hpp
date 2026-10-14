#pragma once

#include "search_thread.h"

#include <memory>
#include <vector>
#include <wx/dataview.h>

/// A node in the "Find In Files" results tree: a file (the parent) or a match (a child of a file)
struct FindResultsNode {
    enum class Kind {
        kFile,
        kMatch,
    };

    /// The result of a replace operation (the "Replace In Files" tab)
    enum class State {
        kPending,
        kDone,
        kFailed,
    };

    explicit FindResultsNode(Kind kind)
        : kind(kind)
    {
    }

    bool IsFile() const { return kind == Kind::kFile; }
    bool IsMatch() const { return kind == Kind::kMatch; }

    Kind kind;
    FindResultsNode* parent{nullptr}; // nullptr for a file
    State state{State::kPending};
    bool checked{false};                                    // "Replace In Files": is this match selected?
    SearchResult result;                                    // a match. For a file, only the file name is set
    std::vector<std::unique_ptr<FindResultsNode>> children; // the matches of a file
};

/// The model of the results tree: the files are the top level items and their matches are the children
class FindResultsModel : public wxDataViewModel
{
public:
    enum {
        kColCheck = 0, // a check box (used by the "Replace In Files" tab)
        kColText = 1,  // the file name or the match (drawn by FindResultsRenderer)
        kColCount,
    };

    FindResultsModel() = default;

    static wxDataViewItem ToItem(const FindResultsNode* node)
    {
        return wxDataViewItem(const_cast<FindResultsNode*>(node));
    }
    static FindResultsNode* ToNode(const wxDataViewItem& item) { return static_cast<FindResultsNode*>(item.GetID()); }

    /// Remove all the results
    void Clear();

    /// Add matches. A match is added to the last file if it belongs to it, otherwise a new file is added
    void AddResults(const SearchResultList& results);

    /// Remove matches (and the files that have no matches left). The nodes are deleted
    void RemoveMatches(const std::vector<FindResultsNode*>& nodes);

    /// Check / uncheck all the matches
    void SetAllChecked(bool checked);

    /// Notify the view that the content of `node` was changed
    void NodeChanged(const FindResultsNode* node);

    bool IsEmpty() const { return m_files.empty(); }
    size_t GetMatchCount() const { return m_matchCount; }
    std::vector<FindResultsNode*> GetFiles() const;
    /// All the matches, in the order they are displayed
    std::vector<FindResultsNode*> GetMatches() const;

    // wxDataViewModel
    unsigned int GetColumnCount() const override { return kColCount; }
    wxString GetColumnType(unsigned int col) const override;
    void GetValue(wxVariant& variant, const wxDataViewItem& item, unsigned int col) const override;
    bool SetValue(const wxVariant& variant, const wxDataViewItem& item, unsigned int col) override;
    wxDataViewItem GetParent(const wxDataViewItem& item) const override;
    bool IsContainer(const wxDataViewItem& item) const override;
    unsigned int GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const override;
    // The files have a value in the check column as well
    bool HasContainerColumns(const wxDataViewItem& item) const override
    {
        wxUnusedVar(item);
        return true;
    }

private:
    std::vector<std::unique_ptr<FindResultsNode>> m_files;
    size_t m_matchCount{0};
};

/// The colours used for drawing the results. They are taken from the current lexer theme
struct FindResultsColours {
    wxColour text;            // the matched line
    wxColour lineNumber;      // the line number (and the number of matches of a file)
    wxColour file;            // the file name
    wxColour matchBackground; // the background of the matched text
    wxColour matchText;       // the matched text
    wxColour failed;          // a match that was not replaced
};

/// Draws a file (its name and the number of matches) or a match (the line number and the line, with the matched text
/// highlighted)
class FindResultsRenderer : public wxDataViewCustomRenderer
{
public:
    FindResultsRenderer();

    void SetColours(const FindResultsColours& colours) { m_colours = colours; }

    bool SetValue(const wxVariant& value) override;
    bool GetValue(wxVariant& value) const override;
    wxSize GetSize() const override;
    bool Render(wxRect rect, wxDC* dc, int state) override;

private:
    const FindResultsNode* m_node{nullptr};
    FindResultsColours m_colours;
};

/// The results tree control
class FindResultsView : public wxDataViewCtrl
{
public:
    /// If `checkable` is true, each file and match has a check box (the "Replace In Files" tab)
    FindResultsView(wxWindow* parent, bool checkable);
    ~FindResultsView() override;

    FindResultsModel* GetResultsModel() const { return m_model.get(); }

    /// Use the colours and the font of the current lexer theme
    void ApplyTheme();

    void ExpandAllFiles();
    void CollapseAllFiles();
    /// Collapse all the files, or expand them all if they are all collapsed
    void ToggleFolding();
    /// Expand only the first file
    void ExpandFirstFileOnly();

    /// Select `node`, expand its file if needed and scroll to it
    void SelectNode(const FindResultsNode* node);

    /// The text of `node` as it is displayed: the file name or "<line>: <text>"
    static wxString GetNodeText(const FindResultsNode* node);
    /// The text of the selected nodes, one line for each
    wxString GetSelectedText() const;

private:
    wxObjectDataPtr<FindResultsModel> m_model;
    FindResultsRenderer* m_renderer{nullptr}; // owned by the column
};
