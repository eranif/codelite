#ifndef CLEDITOR_CONFIG_H
#define CLEDITOR_CONFIG_H

#include "codelite_exports.h"
#include <wx/string.h>
#include <wx/filename.h>

class WXDLLIMPEXP_CL clEditorConfig
{
protected:
    wxString indent_style;
    size_t indent_size;
    size_t tab_width;
    wxString charset;
    bool trim_trailing_whitespace;
    bool insert_final_newline;
    wxString end_of_line;

public:
    clEditorConfig();
    ~clEditorConfig();

    /**
     * @brief try and load a .editorconfig settings for 'filename'. We start looking from the current file location
     * and going up until we hit the root folder
     */
    bool LoadForFile(const wxFileName& filename);

    void SetCharset(const wxString& charset) { this->charset = charset; }
    void SetIndentSize(size_t indent_size) { this->indent_size = indent_size; }
    void SetIndentStyle(const wxString& indent_style) { this->indent_style = indent_style; }
    void SetInsertFinalNewline(bool insert_final_newline) { this->insert_final_newline = insert_final_newline; }
    void SetTabWidth(size_t tab_width) { this->tab_width = tab_width; }
    void SetTrimTrailingWhitespace(bool trim_trailing_whitespace)
    {
        this->trim_trailing_whitespace = trim_trailing_whitespace;
    }

    const wxString& GetCharset() const { return charset; }
    size_t GetIndentSize() const { return indent_size; }
    const wxString& GetIndentStyle() const { return indent_style; }
    bool IsInsertFinalNewline() const { return insert_final_newline; }
    size_t GetTabWidth() const { return tab_width; }
    bool IsTrimTrailingWhitespace() const { return trim_trailing_whitespace; }
    void SetEndOfLine(const wxString& end_of_line) { this->end_of_line = end_of_line; }
    const wxString& GetEndOfLine() const { return end_of_line; }
};

#endif // CLEDITOR_CONFIG_H
