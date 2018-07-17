#include "csFindInFilesCommandHandler.h"

csFindInFilesCommandHandler::csFindInFilesCommandHandler(wxEvtHandler* sink)
    : csCommandHandlerBase(sink)
{
}

csFindInFilesCommandHandler::~csFindInFilesCommandHandler() {}

void csFindInFilesCommandHandler::Process(const JSONElement& options)
{
    // Extract the options
    CHECK_OPTION("folder");
    CHECK_OPTION("what");
    CHECK_OPTION("mask");
    CHECK_OPTION("case-sensitive");
    CHECK_OPTION("whole-word");

    wxString folder = options.namedObject("folder").toString();
    wxString what = options.namedObject("what").toString();
    wxString mask = options.namedObject("mask").toString();
    bool caseSensitive = options.namedObject("case-sensitive").toBool(false);
    bool word = options.namedObject("whole-word").toBool(false);

    if(folder.IsEmpty() || !wxFileName::DirExists(folder)) {
        clERROR() << "Invalid input directory:" << folder;
        NotifyCompletion();
        return;
    }
    if(what.IsEmpty()) {
        clERROR() << "what field is empty";
        NotifyCompletion();
        return;
    }
}
