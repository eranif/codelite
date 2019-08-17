#include "csFindInFilesCommandHandler.h"
#include "search_thread.h"
#include "csManager.h"

csFindInFilesCommandHandler::csFindInFilesCommandHandler(csManager* manager)
    : csCommandHandlerBase(manager)
{
}

csFindInFilesCommandHandler::~csFindInFilesCommandHandler() {}

void csFindInFilesCommandHandler::DoProcessCommand(const JSONItem& options)
{
    // Extract the options
    CHECK_STR_PARAM("path", m_folder);
    CHECK_STR_PARAM("what", m_what);
    CHECK_STR_PARAM("mask", m_mask);
    CHECK_BOOL_PARAM("case", m_case);
    CHECK_BOOL_PARAM("word", m_word);

    if(m_folder.IsEmpty() || !wxFileName::DirExists(m_folder)) {
        clERROR() << "Invalid input directory:" << m_folder;
        return;
    }
    if(m_what.IsEmpty()) {
        clERROR() << "what field is empty";
        return;
    }

    // Since we use a background thread to process the data for us
    // we don't want that the base class will notify-completion until the background thread
    // has completed the search. We will do it ourself when the search thread complete its task
    SetNotifyCompletion(false);

    SearchData* req = new SearchData();
    req->SetExtensions(m_mask);
    req->SetFindString(m_what);
    req->SetMatchCase(m_case);
    req->SetMatchWholeWord(m_word);
    wxArrayString folders;
    folders.Add(m_folder);
    req->SetRootDirs(folders);
    req->SetOwner(GetSink());
    SearchThreadST::Get()->Add(req);
}
