#include "wxgui_globals.h"

#include <wx/ffile.h>
#include <wx/log.h>

namespace wxCrafter
{
bool ReadFileContent(const wxString& fileName, wxString& content)
{
    wxLogNull noLog;
    content.Clear();
    wxFFile file(fileName, wxT("rb"));

    if (file.IsOpened()) {

        file.ReadAll(&content);
        if (content.IsEmpty()) {
            // now try the Utf8
            file.ReadAll(&content, wxConvUTF8);
        }
    }
    return !content.IsEmpty();
}

} // namespace wxCrafter
