#include "NodeJSGetScriptHandler.h"
#include "fileutils.h"
#include <wx/filename.h>
#include "json_node.h"
#include "cl_standard_paths.h"
#include "cl_command_event.h"
#include "event_notifier.h"
#include "NodeJSEvents.h"
#include "NodeJSDebugger.h"
#include "file_logger.h"

NodeJSGetScriptHandler::NodeJSGetScriptHandler(const wxString& filename, int line)
    : m_filename(filename)
    , m_line(line)
{
}

NodeJSGetScriptHandler::~NodeJSGetScriptHandler()
{
}

void NodeJSGetScriptHandler::Process(NodeJSDebugger* debugger, const wxString& output)
{
    JSONRoot root(output);
    wxString sourceFile = root.toElement().namedObject("body").namedObject("source").toString();
    if(!sourceFile.IsEmpty()) {
        wxFileName fn(clStandardPaths::Get().GetUserDataDir(), m_filename);
        fn.AppendDir("webtools");
        fn.AppendDir("tmp");
        fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        if(FileUtils::WriteFileContent(fn, sourceFile)) {
            CL_DEBUG("Calling marking line for %s:%d", fn.GetFullPath(), m_line);
            debugger->AddTempFile(fn.GetFullPath());
            clDebugEvent eventHighlight(wxEVT_NODEJS_DEBUGGER_MARK_LINE);
            eventHighlight.SetLineNumber(m_line);
            eventHighlight.SetFileName(fn.GetFullPath());
            EventNotifier::Get()->AddPendingEvent(eventHighlight);
        }
    }
}
