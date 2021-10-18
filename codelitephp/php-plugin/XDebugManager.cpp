#include "XDebugManager.h"
#include "PHPDebugStartDlg.h"
#include "XDebugCommThread.h"
#include "XDebugEvalCmdHandler.h"
#include "XDebugUnknownCommand.h"
#include "php.h"
#include "php_configuration_data.h"
#include "php_utils.h"
#include "php_workspace.h"
#include "ssh_workspace_settings.h"
#include <bookmark_manager.h>
#include <event_notifier.h>
#include <file_logger.h>
#include <plugin.h>
#include <wx/msgdlg.h>
#include <wx/richmsgdlg.h>
#include <wx/sckaddr.h>
#include <wx/sstream.h>
#include <wx/stc/stc.h>
#include <wx/uri.h>
#include <xmlutils.h>

// Handlers
#include "XDebugBreakpointCmdHandler.h"
#include "XDebugContextGetCmdHandler.h"
#include "XDebugPropertyGetHandler.h"
#include "XDebugRunCmdHandler.h"
#include "XDebugStackGetCmdHandler.h"
#include "XDebugStopCmdHandler.h"
#include "xdebugevent.h"

#define CHECK_XDEBUG_SESSION_ACTIVE(event) \
    if(!IsConnected()) {                   \
        event.Skip();                      \
        return;                            \
    }

XDebugManager::XDebugManager()
    : TranscationId(0)
    , m_plugin(NULL)
    , m_readerThread(NULL)
    , m_connected(false)
{
    // Connect CodeLite's debugger events to XDebugManager
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_START, &XDebugManager::OnDebugStartOrContinue, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STOP, &XDebugManager::OnStopDebugger, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_CONTINUE, &XDebugManager::OnDebugStartOrContinue, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_IS_RUNNING, &XDebugManager::OnDebugIsRunning, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &XDebugManager::OnToggleBreakpoint, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_NEXT, &XDebugManager::OnDebugNext, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_NEXT_INST, &XDebugManager::OnVoid, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_IN, &XDebugManager::OnDebugStepIn, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_I, &XDebugManager::OnVoid, this); // Not supported
    EventNotifier::Get()->Bind(wxEVT_DBG_UI_STEP_OUT, &XDebugManager::OnDebugStepOut, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_EXPR_TOOLTIP, &XDebugManager::OnTooltip, this);
    EventNotifier::Get()->Bind(wxEVT_DBG_CAN_INTERACT, &XDebugManager::OnCanInteract, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_IDE_GOT_CONTROL, &XDebugManager::OnGotFocusFromXDebug, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_STOPPED, &XDebugManager::OnXDebugStopped, this);
    EventNotifier::Get()->Bind(wxEVT_PHP_STACK_TRACE_ITEM_ACTIVATED, &XDebugManager::OnStackTraceItemActivated, this);
    EventNotifier::Get()->Bind(wxEVT_PHP_BREAKPOINT_ITEM_ACTIVATED, &XDebugManager::OnBreakpointItemActivated, this);
    EventNotifier::Get()->Bind(wxEVT_PHP_DELETE_ALL_BREAKPOINTS, &XDebugManager::OnDeleteAllBreakpoints, this);
    EventNotifier::Get()->Bind(wxEVT_PHP_DELETE_BREAKPOINT, &XDebugManager::OnDeleteBreakpoint, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_BREAKPOINTS_UPDATED, &XDebugManager::OnBreakpointsViewUpdated, this);
    EventNotifier::Get()->Bind(wxEVT_XDEBUG_EVAL_EXPRESSION, &XDebugManager::OnShowTooltip, this);
}

XDebugManager::~XDebugManager()
{
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_START, &XDebugManager::OnDebugStartOrContinue, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STOP, &XDebugManager::OnStopDebugger, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_CONTINUE, &XDebugManager::OnDebugStartOrContinue, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_IS_RUNNING, &XDebugManager::OnDebugIsRunning, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_TOGGLE_BREAKPOINT, &XDebugManager::OnToggleBreakpoint, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_NEXT, &XDebugManager::OnDebugNext, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_NEXT_INST, &XDebugManager::OnVoid, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STEP_IN, &XDebugManager::OnDebugStepIn, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STEP_I, &XDebugManager::OnVoid, this); // Not supported
    EventNotifier::Get()->Unbind(wxEVT_DBG_UI_STEP_OUT, &XDebugManager::OnDebugStepOut, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_EXPR_TOOLTIP, &XDebugManager::OnTooltip, this);
    EventNotifier::Get()->Unbind(wxEVT_DBG_CAN_INTERACT, &XDebugManager::OnCanInteract, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_IDE_GOT_CONTROL, &XDebugManager::OnGotFocusFromXDebug, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_STOPPED, &XDebugManager::OnXDebugStopped, this);
    EventNotifier::Get()->Unbind(wxEVT_PHP_STACK_TRACE_ITEM_ACTIVATED, &XDebugManager::OnStackTraceItemActivated, this);
    EventNotifier::Get()->Unbind(wxEVT_PHP_BREAKPOINT_ITEM_ACTIVATED, &XDebugManager::OnBreakpointItemActivated, this);
    EventNotifier::Get()->Unbind(wxEVT_PHP_DELETE_ALL_BREAKPOINTS, &XDebugManager::OnDeleteAllBreakpoints, this);
    EventNotifier::Get()->Unbind(wxEVT_PHP_DELETE_BREAKPOINT, &XDebugManager::OnDeleteBreakpoint, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_BREAKPOINTS_UPDATED, &XDebugManager::OnBreakpointsViewUpdated, this);
    EventNotifier::Get()->Unbind(wxEVT_XDEBUG_EVAL_EXPRESSION, &XDebugManager::OnShowTooltip, this);
}

void XDebugManager::OnDebugStartOrContinue(clDebugEvent& e)
{
    if(!PHPWorkspace::Get()->IsOpen()) {
        // Call skip so CodeLite will continue to handle this event
        // by passing it to other debuggers
        e.Skip();
        return;
    }

    // This event is ours to handler, don't call e.Skip()

    // a PHP debug session requested to start
    if(!m_readerThread) {
        // No reader thread is up, start on
        // starting the debugger
        e.SetFeatures(0); // No special features by the PHP debugger
        DoStartDebugger();

    } else {
        // The debugger is already running - issue a 'Continue' command
        DoContinue();
    }
}

void XDebugManager::DoStartDebugger(bool ideInitiate)
{
    CHECK_COND_RET(PHPWorkspace::Get()->GetActiveProject());

    // Test which file we want to debug
    PHPDebugStartDlg debugDlg(EventNotifier::Get()->TopFrame(), PHPWorkspace::Get()->GetActiveProject(),
                              m_plugin->GetManager());
    if(ideInitiate) {
        if(debugDlg.ShowModal() != wxID_OK) {
            return;
        }
    }
    wxDELETE(m_readerThread);
    m_readerThread = new XDebugComThread(this, GetPort(), GetHost(), ideInitiate ? 5 : -1);
    m_readerThread->Start();

    // Starting event
    XDebugEvent eventStarting(wxEVT_XDEBUG_SESSION_STARTING);
    EventNotifier::Get()->ProcessEvent(eventStarting);

    PHPConfigurationData conf;
    conf.Load();
    if(!conf.HasFlag(PHPConfigurationData::kDontPromptForMissingFileMapping) &&
       GetFileMapping(PHPWorkspace::Get()->GetActiveProject()).empty()) {
        // Issue a warning
        wxString message;
        message << _("This project has no file mapping defined. This may result in breakpoints not applied\n")
                << _("To fix this, set file mapping from Project Settings -> Debug");

        wxRichMessageDialog dlg(EventNotifier::Get()->TopFrame(), message, "CodeLite",
                                wxICON_WARNING | wxOK | wxOK_DEFAULT | wxCANCEL);
        dlg.ShowCheckBox(_("Remember my answer and don't show this message again"));
        dlg.SetOKCancelLabels(_("OK, Continue to Debug"), _("Stop the debugger"));
        int dlgResult = dlg.ShowModal();
        conf.EnableFlag(PHPConfigurationData::kDontPromptForMissingFileMapping, dlg.IsCheckBoxChecked()).Save();
        if(dlgResult == wxID_CANCEL) {
            // Stop the debugger
            DoStopDebugger();
            return;
        }
    }

    if(ideInitiate) {
        // Now we can run the project
        if(!PHPWorkspace::Get()->RunProject(true, debugDlg.GetPath(), "", conf.GetXdebugIdeKey())) {
            DoStopDebugger();
            return;
        }
    }
    // Notify about debug session started
    XDebugEvent eventStart(wxEVT_XDEBUG_SESSION_STARTED);
    EventNotifier::Get()->AddPendingEvent(eventStart);

    // Fire CodeLite IDE event indicating that a debug session started
    clDebugEvent cl_event(wxEVT_DEBUG_STARTED);
    EventNotifier::Get()->AddPendingEvent(cl_event);
}

void XDebugManager::OnSocketInput(const std::string& reply) { ProcessDebuggerMessage(reply); }

void XDebugManager::OnDebugIsRunning(clDebugEvent& e)
{
    if(PHPWorkspace::Get()->IsOpen()) {
        e.SetAnswer((m_readerThread != nullptr));
    } else {
        // Not ours to handle
        e.Skip();
    }
}

void XDebugManager::OnStopDebugger(clDebugEvent& e)
{
    if(m_readerThread) {
        // abort
        wxDELETE(m_readerThread);

    } else {
        e.Skip();
    }
}

void XDebugManager::DoStopDebugger()
{
    ClearDebuggerMarker();
    m_connected = false;

    // Clear all handlers from the queue
    m_handlers.clear();

    // Save the breakpoints

    // Reset the connection
    CL_DEBUG("CodeLite >>> closing debug session");
    wxDELETE(m_readerThread);

    // Notify about debug session ended
    XDebugEvent eventEnd(wxEVT_XDEBUG_SESSION_ENDED);
    EventNotifier::Get()->AddPendingEvent(eventEnd);

    {
        clDebugEvent e(wxEVT_DEBUG_ENDED);
        EventNotifier::Get()->AddPendingEvent(e);
    }
}

bool XDebugManager::ProcessDebuggerMessage(const wxString& buffer)
{
    if(buffer.IsEmpty())
        return false;

    CL_DEBUGS(wxString() << "XDebug <<< " << buffer);

    wxXmlDocument doc;
    wxStringInputStream sis(buffer);
    if(!doc.Load(sis)) {
        // failed to parse XML
        CL_DEBUG("CodeLite >>> invalid XML!");
        return false;
    }

    wxXmlNode* root = doc.GetRoot();

    if(root->GetName() == "init") {

        // Parse the content and notify codelite to open the main file
        xInitStruct initData = ParseInitXML(root);

        // Negotiate features with the IDE
        DoNegotiateFeatures();

        // this is a startup message from the debugger
        DoApplyBreakpoints();

        // Issue a "Continue" command
        DoContinue();

    } else if(root->GetName() == "response") {
        // Handle response
        DoHandleResponse(root);
    }
    return true;
}

void XDebugManager::DoApplyBreakpoints()
{
    CL_DEBUG("CodeLite >>> Applying breakpoints");
    if(!m_readerThread) {
        clDEBUG() << "CodeLite (PHP): No XDebug reader thread?" << endl;
        return;
    }

    PHPProject::Ptr_t pProject = PHPWorkspace::Get()->GetActiveProject();
    if(!pProject) {
        clDEBUG() << "CodeLite (PHP): No active project!" << endl;
        return;
    }

    const PHPProjectSettingsData& settings = pProject->GetSettings();
    // bool bRunAsWebserver = (pProject->GetSettings().GetRunAs() == PHPProjectSettingsData::kRunAsWebsite);

    XDebugBreakpoint::List_t& breakpoints = m_breakpointsMgr.GetBreakpoints();
    if(breakpoints.empty()) {
        clDEBUG() << "CodeLite (PHP): No breakpoints to apply" << endl;
        return;
    }

    for(auto& bp : breakpoints) {
        // apply only breakpoints without xdebug-id attached to them
        if(bp.IsApplied()) {
            clDEBUG() << "CodeLite (PHP): Breakpoint already applied" << endl;
            continue;
        }

        wxStringMap_t sftpMapping;
        SSHWorkspaceSettings sftpSettings;
        sftpSettings.Load();
        if(!sftpSettings.GetRemoteFolder().IsEmpty() && sftpSettings.IsRemoteUploadEnabled()) {
            sftpMapping.insert({ PHPWorkspace::Get()->GetFilename().GetPath(), sftpSettings.GetRemoteFolder() });
        }

        wxString command;
        XDebugCommandHandler::Ptr_t handler(new XDebugBreakpointCmdHandler(this, ++TranscationId, bp));
        wxString filepath = settings.GetMappdPath(bp.GetFileName(), true, sftpMapping);
        command << "breakpoint_set -t line -f " << filepath << " -n " << bp.GetLine() << " -i "
                << handler->GetTransactionId();
        DoSocketWrite(command);
        AddHandler(handler);
    }
}

void XDebugManager::DoContinue()
{
    CHECK_PTR_RET(m_readerThread);
    SendRunCommand();
}

void XDebugManager::DoHandleResponse(wxXmlNode* xml)
{
    CHECK_PTR_RET(xml);

    wxString txId = xml->GetAttribute("transaction_id");
    long nTxId(0);
    txId.ToCLong(&nTxId);
    XDebugCommandHandler::Ptr_t handler = PopHandler(nTxId);
    if(handler) {
        handler->Process(xml);

    } else {

        // Just log the reply
        wxXmlDocument doc;
        doc.SetRoot(xml);

        wxStringOutputStream sos;
        if(doc.Save(sos)) {
            CL_DEBUG(sos.GetString());
        }
        doc.DetachRoot();
    }
}

void XDebugManager::DoSocketWrite(const wxString& command)
{
    CHECK_PTR_RET(m_readerThread);
    m_readerThread->SendMsg(command);
}

xInitStruct XDebugManager::ParseInitXML(wxXmlNode* init)
{
    xInitStruct initData;
    wxURI fileuri(init->GetAttribute("fileuri"));
    initData.filename = fileuri.BuildUnescapedURI();
    return initData;
}

void XDebugManager::AddHandler(XDebugCommandHandler::Ptr_t handler)
{
    if(m_handlers.count(handler->GetTransactionId())) {
        m_handlers.erase(handler->GetTransactionId());
    }
    m_handlers.insert(std::make_pair(handler->GetTransactionId(), handler));
}

XDebugCommandHandler::Ptr_t XDebugManager::PopHandler(int transcationId)
{
    XDebugCommandHandler::Ptr_t handler(NULL);
    if(m_handlers.count(transcationId)) {
        handler = m_handlers[transcationId];
        m_handlers.erase(transcationId);
    }
    return handler;
}

void XDebugManager::SendRunCommand()
{
    CHECK_PTR_RET(m_readerThread);
    wxString command;
    XDebugCommandHandler::Ptr_t handler(new XDebugRunCmdHandler(this, ++TranscationId));
    command << "run -i " << handler->GetTransactionId();
    DoSocketWrite(command);
    AddHandler(handler);
}

void XDebugManager::SendStopCommand()
{
    CHECK_PTR_RET(m_readerThread);
    wxString command;
    XDebugCommandHandler::Ptr_t handler(new XDebugStopCmdHandler(this, ++TranscationId));
    command << "stop -i " << handler->GetTransactionId();
    DoSocketWrite(command);
    AddHandler(handler);
}

void XDebugManager::OnToggleBreakpoint(clDebugEvent& e)
{
    if(!PHPWorkspace::Get()->IsOpen()) {
        e.Skip();
        return;
    }

    // User toggled a breakpoint
    IEditor* editor = m_plugin->GetManager()->GetActiveEditor();
    if(editor && editor->GetFileName().GetFullPath() == e.GetFileName()) {
        // Correct editor
        // add marker
        if(m_breakpointsMgr.HasBreakpoint(e.GetFileName(), e.GetInt())) {

            XDebugBreakpoint bp;
            m_breakpointsMgr.GetBreakpoint(e.GetFileName(), e.GetInt(), bp);
            if(bp.IsApplied() && m_readerThread) {
                // Remove it from XDebug as well
                DoDeleteBreakpoint(bp.GetBreakpointId());
            }
            m_breakpointsMgr.DeleteBreakpoint(e.GetFileName(), e.GetInt());

        } else {
            m_breakpointsMgr.AddBreakpoint(e.GetFileName(), e.GetInt());
            DoApplyBreakpoints();
        }
        DoRefreshBreakpointsMarkersForEditor(editor);
    }
}

void XDebugManager::ClearDebuggerMarker()
{
    IEditor::List_t editors;
    m_plugin->GetManager()->GetAllEditors(editors);
    IEditor::List_t::iterator iter = editors.begin();
    for(; iter != editors.end(); ++iter) {
        (*iter)->GetCtrl()->MarkerDeleteAll(smt_indicator);
    }
}

void XDebugManager::SetDebuggerMarker(wxStyledTextCtrl* stc, int lineno)
{
    stc->MarkerDeleteAll(smt_indicator);
    stc->MarkerAdd(lineno, smt_indicator);
    int caretPos = stc->PositionFromLine(lineno);
    stc->SetSelection(caretPos, caretPos);
    stc->SetCurrentPos(caretPos);
    stc->EnsureCaretVisible();
    CenterEditor(stc, lineno);
}

void XDebugManager::OnGotFocusFromXDebug(XDebugEvent& e)
{
    e.Skip();

    // Make sure codelite is "Raised"
    wxFrame* frame = EventNotifier::Get()->TopFrame();
    if(frame->IsIconized() || !frame->IsShown()) {
        frame->Raise();
    }

    CL_DEBUG("CodeLite: opening file %s:%d", e.GetFileName(),
             e.GetLineNumber() + 1); // The user sees the line number from 1 (while scintilla counts them from 0)

    // Mark the debugger line / file
    IEditor* editor = m_plugin->GetManager()->FindEditor(e.GetFileName());
    if(!editor && wxFileName::Exists(e.GetFileName())) {
        // Try to open the editor
        if(m_plugin->GetManager()->OpenFile(e.GetFileName(), "", e.GetLineNumber())) {
            editor = m_plugin->GetManager()->GetActiveEditor();
        }
    }

    if(editor) {
        m_plugin->GetManager()->SelectPage(editor->GetCtrl());
        CallAfter(&XDebugManager::SetDebuggerMarker, editor->GetCtrl(), e.GetLineNumber());
    }

    // Update the callstack/locals views
    DoRefreshDebuggerViews();

    // Re-apply any new breakpoints
    DoApplyBreakpoints();
}

void XDebugManager::OnDebugNext(clDebugEvent& e)
{
    CHECK_XDEBUG_SESSION_ACTIVE(e);
    wxString command;
    XDebugCommandHandler::Ptr_t handler(new XDebugRunCmdHandler(this, ++TranscationId));
    command << "step_over -i " << handler->GetTransactionId();
    DoSocketWrite(command);
    AddHandler(handler);
}

void XDebugManager::OnDebugStepIn(clDebugEvent& e)
{
    CHECK_XDEBUG_SESSION_ACTIVE(e);
    wxString command;
    XDebugCommandHandler::Ptr_t handler(new XDebugRunCmdHandler(this, ++TranscationId));
    command << "step_into -i " << handler->GetTransactionId();
    DoSocketWrite(command);
    AddHandler(handler);
}

void XDebugManager::OnDebugStepOut(clDebugEvent& e)
{
    CHECK_XDEBUG_SESSION_ACTIVE(e);
    wxString command;
    XDebugCommandHandler::Ptr_t handler(new XDebugRunCmdHandler(this, ++TranscationId));
    command << "step_out -i " << handler->GetTransactionId();
    DoSocketWrite(command);
    AddHandler(handler);
}

void XDebugManager::OnVoid(clDebugEvent& e)
{
    CHECK_XDEBUG_SESSION_ACTIVE(e);
    // do nothing
}

void XDebugManager::OnXDebugStopped(XDebugEvent& e)
{
    e.Skip();
    DoStopDebugger();
}

void XDebugManager::DoRefreshDebuggerViews(int requestedStack)
{
    if(!m_readerThread) {
        return;
    }

    // We execute here 2 commands in a row
    {
        wxString command;
        XDebugCommandHandler::Ptr_t handler(new XDebugStackGetCmdHandler(this, ++TranscationId, requestedStack));

        // Get the current stack frames
        command << "stack_get -i " << handler->GetTransactionId();
        DoSocketWrite(command);
        AddHandler(handler);
    }

    {
        wxString command;
        // Get the 'Locals' view
        XDebugCommandHandler::Ptr_t handler(new XDebugContextGetCmdHandler(this, ++TranscationId, requestedStack));
        command << "context_get -d " << requestedStack << " -i " << handler->GetTransactionId();
        DoSocketWrite(command);
        AddHandler(handler);
    }

    // FIXME :: Refresh any watches we have
    // Locals are updated automatically
}

static XDebugManager* s_xdebugManager = NULL;
XDebugManager& XDebugManager::Get()
{
    if(!s_xdebugManager) {
        s_xdebugManager = new XDebugManager();
    }
    return *s_xdebugManager;
}

void XDebugManager::Initialize(PhpPlugin* plugin) { Get().m_plugin = plugin; }

int XDebugManager::GetPort() const
{
    PHPConfigurationData phpGlobalSettings;
    phpGlobalSettings.Load();
    return phpGlobalSettings.GetXdebugPort();
}

wxString XDebugManager::GetHost() const
{
    PHPConfigurationData phpGlobalSettings;
    phpGlobalSettings.Load();
    return phpGlobalSettings.GetXdebugHost();
}

void XDebugManager::Free() { wxDELETE(s_xdebugManager); }

void XDebugManager::DoDeleteBreakpoint(int bpid)
{
    wxString command;
    // Get the 'Locals' view
    command << "breakpoint_remove -i " << ++TranscationId << " -d " << bpid;
    DoSocketWrite(command);
}

void XDebugManager::OnStackTraceItemActivated(PHPEvent& e)
{
    e.Skip();
    wxString filename = e.GetFileName();
    int line = e.GetLineNumber();
    int depth = e.GetInt();

    if(!m_plugin->GetManager()->OpenFile(filename, "", line - 1)) {
        ::wxMessageBox(_("Could not open file: ") + filename, "CodeLite", wxICON_WARNING | wxOK | wxCENTER);
    }
    DoRefreshDebuggerViews(depth);
}

wxStringMap_t XDebugManager::GetFileMapping(PHPProject::Ptr_t pProject) const
{
    wxASSERT(pProject);
    wxStringMap_t mappings;
    const PHPProjectSettingsData& settings = pProject->GetSettings();
    mappings = settings.GetFileMapping();

    // Add the SFTP mappings
    SSHWorkspaceSettings sftpSettings;
    sftpSettings.Load();
    if(!sftpSettings.GetRemoteFolder().IsEmpty() && sftpSettings.IsRemoteUploadEnabled()) {
        mappings.insert(std::make_pair(PHPWorkspace::Get()->GetFilename().GetPath(), sftpSettings.GetRemoteFolder()));
    }
    return mappings;
}

void XDebugManager::OnDeleteAllBreakpoints(PHPEvent& e)
{
    e.Skip();

    // Delete them from XDebug
    const XDebugBreakpoint::List_t& bps = m_breakpointsMgr.GetBreakpoints();
    XDebugBreakpoint::List_t::const_iterator iter = bps.begin();
    for(; iter != bps.end(); ++iter) {
        if(iter->IsApplied()) {
            DoDeleteBreakpoint(iter->GetBreakpointId());
        }
    }

    // Delete them from the manager
    m_breakpointsMgr.DeleteAllBreakpoints();
}

void XDebugManager::OnDeleteBreakpoint(PHPEvent& e)
{
    e.Skip();
    wxString filename = e.GetFileName();
    int line = e.GetLineNumber();
    int bpid = e.GetInt();

    if(bpid != wxNOT_FOUND) {
        // breakpoint was applied
        DoDeleteBreakpoint(bpid);
    }
    IEditor* editor = m_plugin->GetManager()->FindEditor(filename);
    if(editor) {
        editor->GetCtrl()->MarkerDelete(line - 1, smt_breakpoint);
    }
    m_breakpointsMgr.DeleteBreakpoint(filename, line);
}

bool XDebugManager::IsDebugSessionRunning() const { return PHPWorkspace::Get()->IsOpen() && (m_readerThread != NULL); }

void XDebugManager::OnBreakpointItemActivated(PHPEvent& e)
{
    e.Skip();
    if(!m_plugin->GetManager()->OpenFile(e.GetFileName(), "", e.GetLineNumber() - 1)) {
        ::wxMessageBox(_("Could not open file: ") + e.GetFileName(), "CodeLite", wxICON_WARNING | wxOK | wxCENTER);
    }
}

void XDebugManager::OnBreakpointsViewUpdated(XDebugEvent& e)
{
    e.Skip();
    IEditor::List_t editors;
    m_plugin->GetManager()->GetAllEditors(editors, true);
    IEditor::List_t::iterator iter = editors.begin();
    for(; iter != editors.end(); ++iter) {
        DoRefreshBreakpointsMarkersForEditor(*iter);
    }
}

void XDebugManager::DoRefreshBreakpointsMarkersForEditor(IEditor* editor)
{
    CHECK_PTR_RET(editor);
    editor->GetCtrl()->MarkerDeleteAll(smt_breakpoint);

    XDebugBreakpoint::List_t bps;
    m_breakpointsMgr.GetBreakpointsForFile(editor->GetFileName().GetFullPath(), bps);
    XDebugBreakpoint::List_t::const_iterator iter = bps.begin();
    for(; iter != bps.end(); ++iter) {
        editor->GetCtrl()->MarkerAdd(iter->GetLine() - 1, smt_breakpoint);
    }
}

void XDebugManager::DoNegotiateFeatures()
{
    CHECK_PTR_RET(m_readerThread);

    // Set the max children returned by XDebug to 1
    // this is done for performance reasons
    wxString command;

    command.Clear();
    command << "feature_set -n max_depth -v 1 -i " << ++TranscationId;
    DoSocketWrite(command);

    command.Clear();
    command << "feature_set -n max_children -v 1024 -i " << ++TranscationId;
    DoSocketWrite(command);
}

void XDebugManager::SendEvalCommand(const wxString& expression, int evalPurpose)
{
    CHECK_PTR_RET(m_readerThread);

    wxString command;
    XDebugCommandHandler::Ptr_t handler(new XDebugEvalCmdHandler(expression, evalPurpose, this, ++TranscationId));
    command << "eval -i " << handler->GetTransactionId() << " -- " << ::Base64Encode(expression);
    DoSocketWrite(command);
    AddHandler(handler);
}

void XDebugManager::OnTooltip(clDebugEvent& e)
{
    CHECK_XDEBUG_SESSION_ACTIVE(e);

    wxString expression = e.GetString();
    if(expression.IsEmpty())
        return;

    expression.Prepend("print_r(").Append(", true)");
    SendEvalCommand(expression, XDebugEvalCmdHandler::kEvalForTooltip);
}

void XDebugManager::OnCanInteract(clDebugEvent& e)
{
    CHECK_XDEBUG_SESSION_ACTIVE(e);
    e.SetAnswer(true); // Otherwise we would never reach this (see the definition of CHECK_XDEBUG_SESSION_ACTIVE macro)
}

void XDebugManager::CloseDebugSession() { DoStopDebugger(); }

void XDebugManager::OnCommThreadTerminated()
{
    CL_DEBUG("CodeLite >>> Comm Thread: session with XDebug is terminated!");
    DoStopDebugger();
}

void XDebugManager::XDebugNotConnecting()
{
    wxRichMessageDialog dlg(EventNotifier::Get()->TopFrame(), _("XDebug did not connect in a timely manner"),
                            "CodeLite", wxICON_WARNING | wxOK | wxCANCEL_DEFAULT | wxCANCEL);
    dlg.SetOKCancelLabels(_("Run XDebug Test"), _("OK"));
    if(dlg.ShowModal() == wxID_OK) {
        m_plugin->CallAfter(&PhpPlugin::RunXDebugDiagnostics);
    }
    DoStopDebugger();
}

void XDebugManager::OnShowTooltip(XDebugEvent& e)
{
    if(e.GetEvalReason() == XDebugEvalCmdHandler::kEvalForTooltip) {
        wxString tip, title;
        title << e.GetString();

        if(!e.IsEvalSucceeded()) {
            tip << _("Error evaluating expression ");
        } else {
            wxString evaluated = e.GetEvaluted();
            // Reomve extra escapes
            evaluated.Replace("\\n", "\n");
            evaluated.Replace("\\t", "\t");
            evaluated.Replace("\\r", "\r");
            evaluated.Replace("\\v", "\v");
            evaluated.Replace("\\b", "\b");
            tip << evaluated;
            tip.Trim();
        }
        m_plugin->GetManager()->GetActiveEditor()->ShowRichTooltip(tip, title, wxNOT_FOUND);
    } else {
        e.Skip();
    }
}

void XDebugManager::SendDBGPCommand(const wxString& cmd)
{
    CHECK_PTR_RET(m_readerThread);

    wxString command;
    XDebugCommandHandler::Ptr_t handler(new XDebugUnknownCommand(this, ++TranscationId));
    command << cmd << " -i " << handler->GetTransactionId();
    DoSocketWrite(command);
    AddHandler(handler);
}

void XDebugManager::SendGetProperty(const wxString& propertyName)
{
    CHECK_PTR_RET(m_readerThread);

    wxString command;
    XDebugCommandHandler::Ptr_t handler(new XDebugPropertyGetHandler(this, ++TranscationId, propertyName));
    command << "property_get -n " << propertyName << " -i " << handler->GetTransactionId();
    DoSocketWrite(command);
    AddHandler(handler);
}

void XDebugManager::SetConnected(bool connected)
{
    this->m_connected = connected;
    XDebugEvent event(wxEVT_XDEBUG_CONNECTED);
    EventNotifier::Get()->AddPendingEvent(event);
}

void XDebugManager::CenterEditor(wxStyledTextCtrl* ctrl, int lineNo)
{
    // Place the debugger line at the center of the editor view
    int linesOnScreen = ctrl->LinesOnScreen();
    int topLine = lineNo - (linesOnScreen / 2);
    ctrl->SetFirstVisibleLine(topLine);
}

void XDebugManager::StartListener()
{
    if(m_readerThread) {
        // Stop the current session
        DoStopDebugger();
    }

    // Start the listener
    DoStartDebugger(false);
}
