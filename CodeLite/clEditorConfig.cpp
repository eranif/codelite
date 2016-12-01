#include "clEditorConfig.h"
#include <wx/tokenzr.h>
#include "fileutils.h"
#include "file_logger.h"
#include <algorithm>

clEditorConfig::clEditorConfig()
    : indent_style("space")
    , indent_size(4)
    , tab_width(4)
    , charset("utf-8")
    , trim_trailing_whitespace(false)
    , insert_final_newline(false)
{
}

clEditorConfig::~clEditorConfig() {}

bool clEditorConfig::LoadForFile(const wxFileName& filename)
{
    wxFileName editorConfigFile(filename.GetPath(), ".editorconfig");

    bool foundFile = false;
    while(editorConfigFile.GetDirCount()) {
        if(editorConfigFile.FileExists()) {
            foundFile = true;
            break;
        }
        editorConfigFile.RemoveLastDir();
    }

    if(!foundFile) return false;

    wxString content;
    if(!FileUtils::ReadFileContent(editorConfigFile, content)) {
        clDEBUG() << "Failed to read file:" << editorConfigFile << clEndl;
        return false;
    }

    wxArrayString lines = ::wxStringTokenize(content, "\n", wxTOKEN_STRTOK);
    for(size_t i = 0; i < lines.size(); ++i) {
        // Remove comments
        wxString strLine = lines.Item(i);
        strLine = strLine.BeforeFirst('#');
        strLine = strLine.BeforeFirst(';');

        strLine.Trim().Trim(false);

        if(strLine.IsEmpty()) continue;

        // Process the line
        if(strLine.StartsWith("[") && strLine.EndsWith("]")) {
            strLine.RemoveLast().Remove(0, 1); // remove the []
            ProcessSection(strLine);
        } else {
            ProcessDirective(strLine);
        }
    }
    return true;
}

enum eEditorConfigState {
    kEC_STATE_NORMAL,
    kEC_STATE_IN_CURLYGRP,
    kEC_STATE_IN_SQUAREGRP,
};

struct clEditorConfigTreeNode {
    wxString data;
    std::vector<clEditorConfigTreeNode*> children;

    clEditorConfigTreeNode* AddChild(const wxString& data)
    {
        clEditorConfigTreeNode* child = new clEditorConfigTreeNode;
        child->data = data;
        children.push_back(child);
        return child;
    }

public:
    clEditorConfigTreeNode() {}
    ~clEditorConfigTreeNode()
    {
        std::for_each(children.begin(), children.end(), [&](clEditorConfigTreeNode* child) { delete child; });
        children.clear();
    }

public:
    void GetPatterns(wxArrayString& patterns) { DoGetPatterns(this, patterns, data); }
    void AddToAllLeaves(const wxString& pattern) { DoAddToAllLeaves(this, pattern); }
    bool IsEmpty() const { return children.empty(); }

private:
    /**
     * @brief collect the patterns from the leaf nodes
     */
    void DoGetPatterns(clEditorConfigTreeNode* node, wxArrayString& patterns, const wxString& curpattern)
    {
        if(node->children.empty()) {
            // leaf node
            patterns.Add(curpattern + data);
        } else {
            for(size_t i = 0; i < node->children.size(); ++i) {
                DoGetPatterns(node->children.at(i), patterns, curpattern + data);
            }
        }
    }

    /**
     * @brief collect the patterns from the leaf nodes
     */
    void DoAddToAllLeaves(clEditorConfigTreeNode* node, const wxString& pattern)
    {
        if(node->children.empty()) {
            // leaf node
            node->data << pattern;
        } else {
            for(size_t i = 0; i < node->children.size(); ++i) {
                DoAddToAllLeaves(node->children.at(i), pattern);
            }
        }
    }
};

wxArrayString clEditorConfig::ProcessSection(wxString& strLine)
{
    eEditorConfigState state = kEC_STATE_NORMAL;

    clEditorConfigTreeNode* tree = new clEditorConfigTreeNode;
    std::vector<clEditorConfigTreeNode*> trees;

    wxString curpattern;
    while(!strLine.IsEmpty()) {
        wxChar ch = strLine.at(0);
        strLine.Remove(0, 1);
        switch(state) {
        case kEC_STATE_NORMAL:
            switch(ch) {
            case '{':
                state = kEC_STATE_IN_CURLYGRP;
                break;
            case '[':
                state = kEC_STATE_IN_SQUAREGRP;
                break;
            case ',':
                // new pattern
                if(!curpattern.IsEmpty()) {
                    if(tree->IsEmpty()) {
                        tree->AddChild(curpattern);
                    } else {
                        tree->AddToAllLeaves(curpattern);
                    }
                    trees.push_back(tree);
                    tree = new clEditorConfigTreeNode;
                    curpattern.clear();
                }
                break;
            default:
                curpattern << ch;
                break;
            }
            break;
        case kEC_STATE_IN_CURLYGRP: {
            // read the buffer until we hit the closing brace
            wxString buffer;
            if(!ReadUntil('}', strLine, buffer)) {
                return wxArrayString();
            }
            state = kEC_STATE_NORMAL;
            wxArrayString groupPatterns = ProcessSection(buffer);
            if(tree->IsEmpty()) {
                for(size_t i = 0; i < groupPatterns.size(); ++i) {
                    tree->AddChild(groupPatterns.Item(i));
                }
            } else {
                for(size_t i = 0; i < groupPatterns.size(); ++i) {
                    tree->AddToAllLeaves(groupPatterns.Item(i));
                }
            }
            break;
        }
        case kEC_STATE_IN_SQUAREGRP: {
            wxString buffer;
            if(!ReadUntil(']', strLine, buffer)) {
                return wxArrayString();
            }
            state = kEC_STATE_NORMAL;
            break;
        }
        }
    }
    
    // Remainder
    if(!curpattern.IsEmpty()) {
        if(tree->IsEmpty()) {
            tree->AddChild(curpattern);
        } else {
            tree->AddToAllLeaves(curpattern);
        }
        trees.push_back(tree);
    }

    wxArrayString res;
    for(size_t i = 0; i < trees.size(); ++i) {
        wxArrayString patterns;
        trees.at(i)->GetPatterns(patterns);
        res.insert(res.end(), patterns.begin(), patterns.end());
        delete trees.at(i);
    }
    return res;
}

void clEditorConfig::ProcessDirective(wxString& strLine) {}

bool clEditorConfig::ReadUntil(wxChar delim, wxString& strLine, wxString& output)
{
    while(!strLine.IsEmpty()) {
        wxChar ch = strLine.at(0);
        strLine.Remove(0, 1);
        if(ch == delim) {
            return true;
        } else {
            output << ch;
        }
    }
    return false;
}
