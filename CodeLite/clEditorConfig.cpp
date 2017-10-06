#include "clEditorConfig.h"
#include <wx/tokenzr.h>
#include "fileutils.h"
#include "file_logger.h"
#include <algorithm>

clEditorConfig::clEditorConfig()
    : m_rootFileFound(false)
{
}

clEditorConfig::~clEditorConfig() {}

bool clEditorConfig::LoadForFile(const wxFileName& filename, wxFileName& editorConfigFile)
{
    editorConfigFile = wxFileName(filename.GetPath(), ".editorconfig");

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

    clEditorConfigSection section;
    m_sections.push_back(section);
    clEditorConfigSection* cursection = &(m_sections.back());
    wxUnusedVar(cursection); // for debug purposes
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
            clEditorConfigSection section;
            section.patterns = ProcessSection(strLine);
            m_sections.push_back(section);
            cursection = &(m_sections.back());

        } else {
            ProcessDirective(strLine);
        }
    }
    clDEBUG1() << "Using .editorconfig file:" << editorConfigFile << clEndl;
    return true;
}

struct clEditorConfigTreeNode {
    wxString data;
    typedef std::vector<clEditorConfigTreeNode*> Vec_t;
    clEditorConfigTreeNode::Vec_t children;

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
    void Add(const wxString& pattern)
    {
        wxArrayString arr;
        arr.Add(pattern);
        Add(arr);
    }

    void Add(const wxArrayString& patterns)
    {
        clEditorConfigTreeNode::Vec_t leaves;
        DoGetLeaves(this, leaves);

        std::for_each(leaves.begin(), leaves.end(), [&](clEditorConfigTreeNode* leaf) {
            for(size_t i = 0; i < patterns.size(); ++i) {
                leaf->AddChild(patterns.Item(i));
            }
        });
    }

    bool IsEmpty() const { return children.empty(); }

private:
    void DoGetLeaves(clEditorConfigTreeNode* node, clEditorConfigTreeNode::Vec_t& leaves)
    {
        if(node->children.empty()) {
            // leaf node
            leaves.push_back(node);
        } else {
            for(size_t i = 0; i < node->children.size(); ++i) {
                DoGetLeaves(node->children.at(i), leaves);
            }
        }
    }

    /**
     * @brief collect the patterns from the leaf nodes
     */
    void DoGetPatterns(clEditorConfigTreeNode* node, wxArrayString& patterns, const wxString& curpattern)
    {
        if(node->children.empty()) {
            // leaf node
            patterns.Add(curpattern + node->data);
        } else {
            for(size_t i = 0; i < node->children.size(); ++i) {
                DoGetPatterns(node->children.at(i), patterns, curpattern + node->data);
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

enum eEditorConfigState {
    kEC_STATE_NORMAL,
    kEC_STATE_IN_CURLYGRP,
    kEC_STATE_IN_SQUAREGRP,
};

wxArrayString clEditorConfig::ProcessSection(wxString& strLine)
{
    eEditorConfigState state = kEC_STATE_NORMAL;

    clEditorConfigTreeNode* tree = new clEditorConfigTreeNode;
    std::vector<clEditorConfigTreeNode*> trees;
    trees.push_back(tree);

    wxString curpattern;
    while(!strLine.IsEmpty()) {
        switch(state) {
        case kEC_STATE_NORMAL: {
            wxChar ch = strLine.at(0);
            strLine.Remove(0, 1);
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
                    tree->Add(curpattern);

                    tree = new clEditorConfigTreeNode;
                    trees.push_back(tree);
                    curpattern.clear();
                }
                break;
            default:
                curpattern << ch;
                break;
            }
            break;
        }
        case kEC_STATE_IN_CURLYGRP: {
            // if we got something so far, add it before we continue
            if(!curpattern.IsEmpty()) {
                tree->Add(curpattern);
                curpattern.clear();
            }

            // read the buffer until we hit the closing brace
            wxString buffer;
            if(!ReadUntil('}', strLine, buffer)) {
                return wxArrayString();
            }
            state = kEC_STATE_NORMAL;
            wxArrayString groupPatterns = ProcessSection(buffer);
            tree->Add(groupPatterns);
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
        tree->Add(curpattern);
    }

    wxArrayString res;
    for(size_t i = 0; i < trees.size(); ++i) {
        wxArrayString patterns;
        trees.at(i)->GetPatterns(patterns);
        res.insert(res.end(), patterns.begin(), patterns.end());
        delete trees.at(i);
    }

    // Loop over the array and change "**" => "*"
    for(size_t i = 0; i < res.size(); ++i) {
        res.Item(i).Replace("**", "*");
    }
    return res;
}

#define IS_TRUE(value) ((value.CmpNoCase("true") == 0) || (value.CmpNoCase("yes") == 0) || (value.CmpNoCase("1") == 0))
void clEditorConfig::ProcessDirective(wxString& strLine)
{
    clEditorConfigSection* cursection = &(m_sections.back());
    wxString key = strLine.BeforeFirst('=');
    wxString value = strLine.AfterFirst('=');
    key.Trim().Trim(false);
    value.Trim().Trim(false);

    if(key == "indent_style") {
        cursection->SetIndentStyle(value.Lower());

    } else if(key == "indent_size") {
        long lv = 4;
        value.ToCLong(&lv);
        cursection->SetIndentSize(lv);

    } else if(key == "tab_width") {
        long lv = 4;
        value.ToCLong(&lv);
        cursection->SetTabWidth(lv);

    } else if(key == "charset") {
        cursection->SetCharset(value.Lower());

    } else if(key == "trim_trailing_whitespace") {
        cursection->SetTrimTrailingWhitespace(IS_TRUE(value));

    } else if(key == "insert_final_newline") {
        cursection->SetInsertFinalNewline(IS_TRUE(value));

    } else if(key == "end_of_line") {
        cursection->SetEndOfLine(value.Lower());

    } else if(key == "root") {
        m_rootFileFound = IS_TRUE(value);
    }
}

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

bool clEditorConfig::GetSectionForFile(const wxFileName& filename, clEditorConfigSection& section)
{
    wxFileName editorConfigFile;
    if(!LoadForFile(filename, editorConfigFile)) return false;
    section = clEditorConfigSection();
    section.filename = editorConfigFile;
    bool match_found = false;
    std::for_each(m_sections.begin(), m_sections.end(), [&](const clEditorConfigSection& sec) {
        for(size_t i = 0; i < sec.patterns.size(); ++i) {
            const wxString& pattern = sec.patterns.Item(i);
            bool is_wild = pattern.Contains("*");

            wxString fullpath = filename.GetFullPath(wxPATH_UNIX);
            wxString fullname = filename.GetFullName();

            if((is_wild && ::wxMatchWild(pattern, fullpath, false)) || (!is_wild && fullname == pattern)) {
                match_found = true;
                if(sec.IsCharsetSet()) {
                    section.SetCharset(sec.GetCharset());
                }
                if(sec.IsIndentSizeSet()) {
                    section.SetIndentSize(sec.GetIndentSize());
                }
                if(sec.IsIndentStyleSet()) {
                    section.SetIndentStyle(sec.GetIndentStyle());
                }
                if(sec.IsInsertFinalNewlineSet()) {
                    section.SetInsertFinalNewline(sec.IsInsertFinalNewline());
                }
                if(sec.IsSetEndOfLineSet()) {
                    section.SetEndOfLine(sec.GetEndOfLine());
                }
                if(sec.IsTabWidthSet()) {
                    section.SetTabWidth(sec.GetTabWidth());
                }
                if(sec.IsTrimTrailingWhitespaceSet()) {
                    section.SetTrimTrailingWhitespace(sec.IsTrimTrailingWhitespace());
                }
                break;
            }
        }
    });

    // Print the match to the log file
    if(match_found) {
        section.PrintToLog();
    }
    return match_found;
}

void clEditorConfigSection::PrintToLog()
{
    clDEBUG1() << ".editorconfig (" << filename << ") :" << clEndl;
    if(IsCharsetSet()) {
        clDEBUG1() << "charset:" << GetCharset() << clEndl;
    }
    if(IsIndentSizeSet()) {
        clDEBUG1() << "indent_size:" << GetIndentSize() << clEndl;
    }
    if(IsIndentStyleSet()) {
        clDEBUG1() << "indent_style:" << GetIndentStyle() << clEndl;
    }
    if(IsInsertFinalNewlineSet()) {
        clDEBUG1() << "insert_final_newline:" << IsInsertFinalNewline() << clEndl;
    }
    if(IsSetEndOfLineSet()) {
        clDEBUG1() << "end_of_line:" << GetEndOfLine() << clEndl;
    }
    if(IsTabWidthSet()) {
        clDEBUG1() << "tab_width:" << GetTabWidth() << clEndl;
    }
    if(IsTrimTrailingWhitespaceSet()) {
        clDEBUG1() << "trim_trailing_whitespace:" << IsTrimTrailingWhitespace() << clEndl;
    }
}
