#include "csListCommandHandler.h"
#include "json_node.h"
#include <file_logger.h>
#include <wx/dir.h>

csListCommandHandler::csListCommandHandler() {}

csListCommandHandler::~csListCommandHandler() {}

void csListCommandHandler::Process(const clNetworkMessage& message, clSocketBase* conn)
{
    clDEBUG() << "Processing list command...";
    const wxString& folder = message["folder"];
    wxDir dir(folder);
    wxString filename;
    bool cont = dir.GetFirst(&filename);
    JSONRoot json(cJSON_Array);
    JSONElement arr = json.toElement();
    while(cont) {
        wxFileName fn(folder, filename);
        JSONElement entry = JSONElement::createObject();
        wxString fullpath = fn.GetFullPath();
        entry.addProperty("path", fn.GetFullPath());
        entry.addProperty("type", wxFileName::DirExists(fullpath) ? "dir" : "file");
        arr.arrayAppend(entry);
        cont = dir.GetNext(&filename);
    }
    conn->WriteMessage(arr.FormatRawString(false));
}
