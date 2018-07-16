#include "csListCommandHandler.h"
#include "json_node.h"
#include <file_logger.h>
#include <iostream>
#include <wx/dir.h>

csListCommandHandler::csListCommandHandler(wxEvtHandler* sink)
    : csCommandHandlerBase(sink)
{
}

csListCommandHandler::~csListCommandHandler() {}

void csListCommandHandler::Process(const wxString& message)
{
    clDEBUG() << "Processing list command...";
    JSONRoot command(message);
    JSONElement e = command.toElement();
    if(!e.hasNamedObject("folder")) {
        clERROR() << "list command must include the 'folder' parameter";
        NotifyCompletion();
        return;
    }
    wxString folder = e.namedObject("folder").toString();

    // Prepare the output
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
    char* result = arr.FormatRawString(false);
    clDEBUG() << result;
    std::cout << result << std::endl;
    free(result);

    NotifyCompletion();
}
