#include "clEditorConfig.h"

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
    return true;
}
