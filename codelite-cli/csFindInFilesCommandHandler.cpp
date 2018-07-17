#include "csFindInFilesCommandHandler.h"
#include "search_thread.h"

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
    CHECK_OPTION("case");
    CHECK_OPTION("word");

    wxString folder = options.namedObject("folder").toString();
    wxString what = options.namedObject("what").toString();
    wxString mask = options.namedObject("mask").toString();
    bool caseSensitive = options.namedObject("case").toBool(false);
    bool word = options.namedObject("word").toBool(false);

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

    SearchData* req = new SearchData();
    req->SetExtensions(mask);
    req->SetFindString(what);
    req->SetMatchCase(caseSensitive);
    req->SetMatchWholeWord(word);
    wxArrayString folders;
    folders.Add(folder);
    req->SetRootDirs(folders);
    req->SetOwner(GetSink());
    SearchThreadST::Get()->Add(req);
}
