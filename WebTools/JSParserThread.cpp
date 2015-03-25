#include "JSParserThread.h"
#include <wx/filename.h>
#include <wx/dir.h>
#include <wx/arrstr.h>
#include "fileutils.h"
#include "JSSourceFile.h"
#include "JSCodeCompletion.h"
#include "JSLookUpTable.h"

JSParserThread::JSParserThread(JSCodeCompletion* jsCC)
    : m_jsCC(jsCC)
{
}

JSParserThread::~JSParserThread() {}

void JSParserThread::ParseFiles(const wxString& folder)
{
    if(!wxFileName::DirExists(folder)) return;
    Request* r = new Request();
    r->path = folder;
    Add(r);
}

void JSParserThread::ProcessRequest(ThreadRequest* request)
{
    Request* r = dynamic_cast<Request*>(request);
    if(!r) return;

    wxArrayString files;
    wxDir::GetAllFiles(r->path, &files, "*.js");

    // Parse the files
    JSLookUpTable::Ptr_t lookup(new JSLookUpTable());
    for(size_t i = 0; i < files.GetCount(); ++i) {
        wxString fileContent;
        FileUtils::ReadFileContent(files.Item(i), fileContent);
        JSSourceFile source(lookup, fileContent, wxFileName(files.Item(i)));
        source.Parse();
    }

    Reply* reply = new Reply;
    reply->lookup = new JSLookUpTable();
    lookup->GetObjects().swap(reply->lookup->GetObjects());
    lookup->Clear();
    m_jsCC->CallAfter(&JSCodeCompletion::PraserThreadCompleted, reply);
}
