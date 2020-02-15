#include "CxxPreProcessor.h"
#include "CxxUsingNamespaceCollector.h"
#include "CxxUsingNamespaceCollectorThread.h"
#include "code_completion_manager.h"
#include "file_logger.h"
#include "macros.h"

CxxUsingNamespaceCollectorThread::CxxUsingNamespaceCollectorThread()
    : WorkerThread()
{
}

CxxUsingNamespaceCollectorThread::~CxxUsingNamespaceCollectorThread() {}

void CxxUsingNamespaceCollectorThread::ProcessRequest(ThreadRequest* request)
{
    CxxUsingNamespaceCollectorThread::Request* req = dynamic_cast<CxxUsingNamespaceCollectorThread::Request*>(request);
    CHECK_PTR_RET(req);

    CxxPreProcessor pp;
    pp.SetMaxDepth(20);
    CxxUsingNamespaceCollector collector(&pp, req->filename);
    for(size_t i = 0; i < req->includePaths.GetCount(); ++i) {
        pp.AddIncludePath(req->includePaths.Item(i));
    }

    clDEBUG1() << "Collecting 'using namespace' statements for file" << req->filename << "started";
    collector.Parse();
    clDEBUG1() << "Collecting 'using namespace' statements for file" << req->filename << "completed";

    CodeCompletionManager::Get().CallAfter(&CodeCompletionManager::OnFindUsingNamespaceDone,
                                           collector.GetUsingNamespaces(), req->filename);
}

void CxxUsingNamespaceCollectorThread::QueueFile(const wxString& filename, const wxArrayString& searchPaths)
{
    CxxUsingNamespaceCollectorThread::Request* req = new CxxUsingNamespaceCollectorThread::Request();
    req->filename = filename;
    req->includePaths = searchPaths;
    Add(req);
}
