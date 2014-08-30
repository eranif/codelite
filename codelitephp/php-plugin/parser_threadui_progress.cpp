#include "parser_threadui_progress.h"
#include "php_workspace_view.h"

ParserThreadUIProgress::ParserThreadUIProgress(PHPWorkspaceView* view)
    : m_view(view)
{
}

ParserThreadUIProgress::~ParserThreadUIProgress()
{
}

void ParserThreadUIProgress::OnProgress(size_t currentIndex, size_t total)
{
    m_view->ReportParseThreadProgress( currentIndex, total );
}

void ParserThreadUIProgress::OnCompleted()
{
    m_view->ReportParseThreadDone();
}
