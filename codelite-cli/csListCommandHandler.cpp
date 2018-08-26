#include "csListCommandHandler.h"
#include "json_node.h"
#include <file_logger.h>
#include <iostream>
#include <wx/dir.h>
#include "csManager.h"

csListCommandHandler::csListCommandHandler(csManager* manager)
    : csCommandHandlerBase(manager)
{
}

csListCommandHandler::~csListCommandHandler() {}

void csListCommandHandler::DoProcessCommand(const JSONElement& options)
{
    clDEBUG() << "Processing list command...";
    CHECK_STR_PARAM("path", m_folder);

    // Prepare the output
    wxDir dir(m_folder);
    wxString filename;
    bool cont = dir.GetFirst(&filename);
    JSONRoot json(cJSON_Array);
    JSONElement arr = json.toElement();
    while(cont) {
        wxFileName fn(m_folder, filename);
        JSONElement entry = JSONElement::createObject();
        wxString fullpath = fn.GetFullPath();
        entry.addProperty("path", fn.GetFullPath());
        entry.addProperty("type", wxFileName::DirExists(fullpath) ? "dir" : "file");
        arr.arrayAppend(entry);
        cont = dir.GetNext(&filename);
    }
    char* result = arr.FormatRawString(m_manager->GetConfig().IsPrettyJSON());
    clDEBUG() << result;
    std::cout << result << std::endl;
    free(result);
}
