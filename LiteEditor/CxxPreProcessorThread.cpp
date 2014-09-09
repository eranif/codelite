#include "CxxPreProcessorThread.h"
#include "CxxPreProcessor.h"
#include "CxxLexerAPI.h"
#include "code_completion_manager.h"
#include "file_logger.h"

CxxPreProcessorThread::CxxPreProcessorThread()
{
}

CxxPreProcessorThread::~CxxPreProcessorThread()
{
}

void CxxPreProcessorThread::ProcessRequest(ThreadRequest* request)
{
    CxxPreProcessorThread::Request* req = dynamic_cast<CxxPreProcessorThread::Request*>(request);
    CHECK_PTR_RET(req);

    CxxPreProcessor pp;
    for(size_t i = 0; i < req->includePaths.GetCount(); ++i) {
        pp.AddIncludePath(req->includePaths.Item(i));
    }

    for(size_t i = 0; i < req->definitions.GetCount(); ++i) {
        pp.AddDefinition(req->definitions.Item(i));
    }

    CL_DEBUG("Parsing of file: %s started\n", req->filename);
    pp.Parse(req->filename, kLexerOpt_CollectMacroValueNumbers | kLexerOpt_DontCollectMacrosDefinedInThisFile);
    CL_DEBUG("Parsing of file: %s completed\n", req->filename);

    CodeCompletionManager::Get().CallAfter(
        &CodeCompletionManager::OnParseThreadCollectedMacros, pp.GetDefinitions(), req->filename);
}

void CxxPreProcessorThread::QueueFile(const wxString& filename,
                                      const wxArrayString& definitions,
                                      const wxArrayString& includePaths)
{
    CxxPreProcessorThread::Request* req = new CxxPreProcessorThread::Request();
    req->definitions = definitions;
    req->includePaths = includePaths;
    req->filename = filename;
    Add(req);
}
