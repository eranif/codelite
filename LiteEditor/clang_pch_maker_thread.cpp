#include "clang_pch_maker_thread.h"
#include <wx/thread.h>

#ifdef __WXMSW__
static wxString PRE_PROCESS_CMD = wxT("\"$CLANG\" -cc1 -fcxx-exceptions $ARGS -w \"$SRC_FILE\" -E 1> \"$PP_OUTPUT_FILE\" 2>&1");
static wxString PCH_CMD         = wxT("\"$CLANG\" -cc1 -fcxx-exceptions -x c++-header $ARGS -w \"$SRC_FILE\" -emit-pch -o \"$PCH_FILE\"");
static wxString CC_CMD          = wxT("\"$CLANG\" -cc1 -fcxx-exceptions $ARGS -w -fsyntax-only -include-pch \"$PCH_FILE\" -code-completion-at=$LOCATION \"$SRC_FILE\"");
#else
static wxString PRE_PROCESS_CMD = wxT("\"$CLANG\" -cc1 -fexceptions $ARGS -w \"$SRC_FILE\" -E 1> \"$PP_OUTPUT_FILE\" 2>&1");
static wxString PCH_CMD         = wxT("\"$CLANG\" -cc1 -fexceptions -x c++-header $ARGS -w \"$SRC_FILE\" -emit-pch -o \"$PCH_FILE\"");
static wxString CC_CMD          = wxT("\"$CLANG\" -cc1 -fexceptions $ARGS -w -fsyntax-only -include-pch \"$PCH_FILE\" -code-completion-at=$LOCATION \"$SRC_FILE\"");
#endif

ClangPchMakerThread::ClangPchMakerThread()
{
}

ClangPchMakerThread::~ClangPchMakerThread()
{
}

void ClangPchMakerThread::ProcessRequest(ThreadRequest* request)
{
	ClangPchCreateTask *task = dynamic_cast<ClangPchCreateTask*>( request );
	if( !task ) {
		return;
	}
	
	switch(task->GetTaskType()) {
	case ClangPchCreateTask::CreatePch:
		DoCreatePch(task);
		break;
		
	default:
		break;
	}
}

bool ClangPchMakerThread::findEntry(const wxString& filename, const wxArrayString &includes)
{
	wxCriticalSectionLocker locker(m_cs);
	const ClangPCHEntry &entry = m_cache.GetPCH(filename);
	bool isOk = entry.IsValid() && entry.NeedRegenration(includes);
	return isOk;
}

void ClangPchMakerThread::DoCreatePch(ClangPchCreateTask *task)
{
	// first, pre-process the file
	
}


