#include "php_parser_thread.h"
#include "PHPLookupTable.h"
#include "PHPSourceFile.h"
#include "macros.h"
#include <wx/dir.h>

PHPParserThread* PHPParserThread::ms_instance = 0;
bool PHPParserThread::ms_goingDown = false;

PHPParserThread::PHPParserThread() {}

PHPParserThread::~PHPParserThread() {}

PHPParserThread* PHPParserThread::Instance()
{
    if(ms_instance == 0) {
        ms_instance = new PHPParserThread();
    }
    return ms_instance;
}

void PHPParserThread::Release()
{
    ms_instance->Stop();
    if(ms_instance) {
        delete ms_instance;
    }
    ms_instance = 0;
    ms_goingDown = false;
}

void PHPParserThread::ProcessRequest(ThreadRequest* request)
{
    PHPParserThreadRequest* r = dynamic_cast<PHPParserThreadRequest*>(request);
    if(r) {
        switch(r->requestType) {
        case PHPParserThreadRequest::kParseWorkspaceFilesFull:
        case PHPParserThreadRequest::kParseWorkspaceFilesQuick:
            ParseFiles(r);
            break;
        case PHPParserThreadRequest::kParseSingleFile:
            ParseFile(r);
            break;
        }
    }
}

void PHPParserThread::ParseFiles(PHPParserThreadRequest* request)
{
    wxFileName fnWorkspaceFile(request->workspaceFile);
    bool isFull = request->requestType == PHPParserThreadRequest::kParseWorkspaceFilesFull;
    wxUnusedVar(isFull);

    wxStringSet_t uniqueFilesSet;
    uniqueFilesSet.insert(request->files.begin(), request->files.end());

    // Open the database
    PHPLookupTable lookuptable;
    lookuptable.Open(fnWorkspaceFile.GetPath());
    lookuptable.RebuildClassCache();
    
    for(size_t i = 0; i < request->frameworksPaths.GetCount(); ++i) {
        if(ms_goingDown) {
            ms_goingDown = false;
            return;
        }
        wxArrayString frameworkFiles;
        wxDir::GetAllFiles(request->frameworksPaths.Item(i), &frameworkFiles, "*.php", wxDIR_DIRS | wxDIR_FILES);
        uniqueFilesSet.insert(frameworkFiles.begin(), frameworkFiles.end());
    }

    // Convert the set back to array
    wxArrayString allFiles;
    wxStringSet_t::iterator iter = uniqueFilesSet.begin();
    for(; iter != uniqueFilesSet.end(); ++iter) {
        allFiles.Add(*iter);
    }

    // Get list of PHP files under
    lookuptable.RecreateSymbolsDatabase(allFiles,
                                        request->requestType == PHPParserThreadRequest::kParseWorkspaceFilesFull ?
                                            PHPLookupTable::kUpdateMode_Full :
                                            PHPLookupTable::kUpdateMode_Fast,
                                        [&]() { return PHPParserThread::ms_goingDown; },
                                        false);
    // reset the shutdown flag
    ms_goingDown = false;
}

void PHPParserThread::ParseFile(PHPParserThreadRequest* request)
{
    wxFileName fnWorkspaceFile(request->workspaceFile);
    PHPLookupTable lookuptable;
    lookuptable.Open(fnWorkspaceFile.GetPath());

    // Parse the source file
    PHPSourceFile sourceFile(wxFileName(request->file), &lookuptable);
    sourceFile.SetParseFunctionBody(false);
    sourceFile.Parse();

    // Save its symbols
    lookuptable.UpdateSourceFile(sourceFile);
}

void PHPParserThread::Clear()
{
    ms_goingDown = true;
    // Clear the queue
    Instance()->m_queue.Clear();
}
