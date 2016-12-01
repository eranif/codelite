#ifndef CLEDITOR_CONFIG_H
#define CLEDITOR_CONFIG_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/filename.h>
#include <vector>

class WXDLLIMPEXP_CL clEditorConfigSection
{
    enum {
        kIndentStyleSet = (1 << 0),
        kIndentSizeSet = (1 << 1),
        kTabWidthSet = (1 << 2),
        kCharsetSet = (1 << 3),
        kTrimTrailingWhitespaceSet = (1 << 4),
        kInsertEOLSet = (1 << 5),
        kEOLStyleSet = (1 << 6),
    };

public:
    wxArrayString patterns;

    size_t flags;

    wxString indent_style;
    size_t indent_size;
    size_t tab_width;
    wxString charset;
    bool trim_trailing_whitespace;
    bool insert_final_newline;
    wxString end_of_line;

    typedef std::vector<clEditorConfigSection> Vec_t;

    clEditorConfigSection()
        : flags(0)
        , indent_style("space")
        , indent_size(4)
        , tab_width(4)
        , charset("utf-8")
        , trim_trailing_whitespace(false)
        , insert_final_newline(false)
    {
    }
    void SetCharset(const wxString& charset)
    {
        this->charset = charset;
        this->flags |= kCharsetSet;
    }
    bool IsCharsetSet() const { return this->flags & kCharsetSet; }

    void SetIndentSize(size_t indent_size)
    {
        this->indent_size = indent_size;
        this->flags |= kIndentSizeSet;
    }
    bool IsIndentSizeSet() const { return this->flags & kIndentSizeSet; }

    void SetIndentStyle(const wxString& indent_style)
    {
        this->indent_style = indent_style;
        this->flags |= kIndentStyleSet;
    }
    bool IsIndentStyleSet() const { return this->flags & kIndentStyleSet; }

    void SetInsertFinalNewline(bool insert_final_newline)
    {
        this->insert_final_newline = insert_final_newline;
        this->flags |= kInsertEOLSet;
    }
    bool IsInsertFinalNewlineSet() const { return this->flags & kInsertEOLSet; }

    void SetTabWidth(size_t tab_width)
    {
        this->tab_width = tab_width;
        this->flags |= kTabWidthSet;
    }
    bool IsTabWidthSet() const { return this->flags & kTabWidthSet; }

    void SetTrimTrailingWhitespace(bool trim_trailing_whitespace)
    {
        this->trim_trailing_whitespace = trim_trailing_whitespace;
        this->flags |= kTrimTrailingWhitespaceSet;
    }
    bool IsTrimTrailingWhitespaceSet() const { return this->flags & kTrimTrailingWhitespaceSet; }

    void SetEndOfLine(const wxString& end_of_line)
    {
        this->end_of_line = end_of_line;
        this->flags |= kEOLStyleSet;
    }
    bool IsSetEndOfLineSet() const { return this->flags & kEOLStyleSet; }

    const wxString& GetCharset() const { return charset; }
    size_t GetIndentSize() const { return indent_size; }
    const wxString& GetIndentStyle() const { return indent_style; }
    bool IsInsertFinalNewline() const { return insert_final_newline; }
    size_t GetTabWidth() const { return tab_width; }
    bool IsTrimTrailingWhitespace() const { return trim_trailing_whitespace; }
    const wxString& GetEndOfLine() const { return end_of_line; }
};

class WXDLLIMPEXP_CL clEditorConfig
{
    clEditorConfigSection::Vec_t m_sections;
    bool m_rootFileFound;

private:
    wxArrayString ProcessSection(wxString& strLine);
    void ProcessDirective(wxString& strLine);
    bool ReadUntil(wxChar delim, wxString& strLine, wxString& output);

public:
    clEditorConfig();
    ~clEditorConfig();

    /**
     * @brief try and load a .editorconfig settings for 'filename'. We start looking from the current file location
     * and going up until we hit the root folder
     */
    bool LoadForFile(const wxFileName& filename);

    /**
     * @brief find the best section for a file
     */
    bool GetSectionForFile(const wxFileName& filename, clEditorConfigSection& section);
};

#endif // CLEDITOR_CONFIG_H
