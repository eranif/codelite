#include "JSCodeCompletion.h"
#include "NodeJSLocator.h"
#include "NoteJSWorkspace.h"
#include "SocketAPI/clSocketClient.h"
#include "WebToolsConfig.h"
#include "asyncprocess.h"
#include "clTernServer.h"
#include "clTernWorkerThread.h"
#include "cl_standard_paths.h"
#include "entry.h"
#include "file_logger.h"
#include "fileutils.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "processreaderthread.h"
#include "wxCodeCompletionBoxEntry.hpp"
#include <wx/log.h>
#include <wx/msgdlg.h>
#include <wx/regex.h>
#include <wx/stc/stc.h>
#include "clNodeJS.h"

clTernServer::clTernServer(JSCodeCompletion* cc)
    : m_jsCCManager(cc)
    , m_tern(NULL)
    , m_goingDown(false)
    , m_workerThread(NULL)
    , m_fatalError(false)
    , m_port(wxNOT_FOUND)
    , m_recycleCount(0)
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &clTernServer::OnTernOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &clTernServer::OnTernTerminated, this);
    m_port = WebToolsConfig::Get().GetPortNumber();
}

clTernServer::~clTernServer() {}

void clTernServer::OnTernOutput(clProcessEvent& event)
{
    PrintMessage(event.GetOutput());
}

static size_t crash_count = 0;
void clTernServer::OnTernTerminated(clProcessEvent& event)
{
    wxDELETE(m_tern);
    if(m_goingDown || !m_jsCCManager->IsEnabled()) { return; }
#if defined(__WXMSW__) && !defined(NDEBUG)
    HANDLE hProcess = ::GetCurrentProcess();
    DWORD handleCount;
    ::GetProcessHandleCount(hProcess, &handleCount);
    clDEBUG1() << "Tern process termianted. Number of handles:" << (int)handleCount;
    ::CloseHandle(hProcess);
#endif

    PrintMessage("Tern server terminated, will restart it\n");
    ++crash_count;
    Start(m_workingDirectory);
}

bool clTernServer::Start(const wxString& workingDirectory)
{
    if(m_fatalError) return false;
    if(!m_jsCCManager->IsEnabled()) return true;
    if(!WebToolsConfig::Get().IsNodeInstalled()) { return true; }
    if(!WebToolsConfig::Get().IsTernInstalled()) { return true; }
    
    m_workingDirectory = workingDirectory;
    WebToolsConfig& conf = WebToolsConfig::Get();

    wxString nodeExe = conf.GetNodejs();
    ::WrapWithQuotes(nodeExe);
    
    wxString ternScriptString = conf.GetTernScript().GetFullPath();
    ::WrapWithQuotes(ternScriptString);

    wxString command;
    command << nodeExe << " " << ternScriptString << " --persistent --port " << m_port;

    if(conf.HasJavaScriptFlag(WebToolsConfig::kJSEnableVerboseLogging)) { command << " --verbose"; }

    // Create a .tern-project file
    if(m_workingDirectory.IsEmpty()) { m_workingDirectory = clStandardPaths::Get().GetUserDataDir(); }

    wxFileName ternConfig(m_workingDirectory, ".tern-project");
    wxString content = conf.GetTernProjectFile();
    if(!FileUtils::WriteFileContent(ternConfig, content)) {
        ::wxMessageBox(_("Could not write tern project file: ") + ternConfig.GetFullPath(), "CodeLite",
                       wxICON_ERROR | wxOK | wxCENTER);
        PrintMessage("Could not write tern project file: " + ternConfig.GetFullPath());
        m_fatalError = true;
        return false;
    }

    PrintMessage(wxString() << "Starting: " << command << "\n");
    m_tern = ::CreateAsyncProcess(this, command, IProcessCreateDefault, m_workingDirectory);
    if(!m_tern) {
        PrintMessage("Failed to start Tern server!");
        return false;
    }
    return true;
}

void clTernServer::Terminate()
{
    m_goingDown = true;
    if(m_tern) { m_tern->Terminate(); }
    wxDELETE(m_tern);

    // Stop the worker thread
    if(m_workerThread) { m_workerThread->Stop(); }
    wxDELETE(m_workerThread);
}

bool clTernServer::PostCCRequest(IEditor* editor)
{
    // Sanity
    if(m_workerThread) return false;        // another request is in progress
    if(m_port == wxNOT_FOUND) return false; // don't know tern's port
    ++m_recycleCount;

    wxStyledTextCtrl* ctrl = editor->GetCtrl();

    // Prepare the request
    JSON root(cJSON_Object);
    JSONItem query = JSONItem::createObject("query");
    root.toElement().append(query);
    query.addProperty("type", wxString("completions"));
    query.addProperty("file", wxString("#0"));
    query.append(CreateLocation(ctrl));
    query.addProperty("docs", true);
    query.addProperty("urls", true);
    query.addProperty("includeKeywords", true);
    query.addProperty("types", true);

    JSONItem files = CreateFilesArray(editor);
    root.toElement().append(files);

    clTernWorkerThread::Request* req = new clTernWorkerThread::Request;
    req->jsonRequest = root.toElement().FormatRawString();
    req->filename = editor->GetFileName().GetFullPath();
    req->type = clTernWorkerThread::kCodeCompletion;

    // Create the worker thread and start the request
    m_workerThread = new clTernWorkerThread(this);
    m_workerThread->Start();
    m_workerThread->Add(req);
    return true;
}

void clTernServer::PrintMessage(const wxString& message)
{
    wxString msg;
    msg << message;
    msg.Trim().Trim(false);
    clDEBUG() << msg;
}

void clTernServer::RecycleIfNeeded(bool force)
{
    if(m_tern && ((m_recycleCount >= 100) || force)) {
        m_recycleCount = 0;
        m_tern->Terminate();

    } else if(!m_tern) {
        // Tern was never started, start it now
        Start(m_workingDirectory);
    }
}

wxString clTernServer::PrepareDoc(const wxString& doc, const wxString& url)
{
    wxString d = doc;
    d.Replace("\\n", " ");

    wxArrayString tokens = ::wxStringTokenize(d, " ", wxTOKEN_STRTOK);
    wxString content;
    wxString curline;
    for(size_t i = 0; i < tokens.size(); ++i) {
        curline << tokens.Item(i) << " ";
        if(curline.length() > 80) {
            content << curline << "\n";
            curline.clear();
        }
    }

    if(!curline.IsEmpty()) { content << curline << "\n"; }
    if(!url.IsEmpty()) { content << "@link " << url; }
    return content;
}

void clTernServer::ProcessType(const wxString& type, wxString& signature, wxString& retValue, int& imgID)
{
    // initialize values
    imgID = wxNOT_FOUND;
    retValue.clear();
    signature.clear();

    if(type.StartsWith("fn(")) {
        imgID = 9; // function
        int depth = 1;
        wxString tmp = type.Mid(3); // skip fn(
        signature = "(";
        bool cont = true;
        while(!tmp.IsEmpty() && cont) {
            wxChar ch = tmp.GetChar(0);
            tmp.Remove(0, 1);
            switch(ch) {
            case '(':
                ++depth;
                signature << "(";
                break;

            case ')':
                --depth;
                if(depth == 0) cont = false;
                signature << ")";
                break;

            default:
                signature << ch;
                break;
            }
        }

        tmp.Trim().Trim(false);
        if(tmp.StartsWith("->")) {
            tmp = tmp.Mid(2);
            retValue = tmp;
        }

    } else {
        imgID = 3; // Member public
        signature.clear();
        retValue = type;
    }
}

void clTernServer::ProcessOutput(const wxString& output, wxCodeCompletionBoxEntry::Vec_t& entries)
{
    // code completion response:
    // ================================
    // {
    // "start": 78,
    // "end": 78,
    // "isProperty": true,
    // "isObjectKey": false,
    // "completions": [
    //   {
    //     "name": "concat",
    //     "type": "fn(other: [?])",
    //     "doc": "Returns a new array comprised of this array joined with other array(s) and/or value(s).",
    //     "url": "https://developer.mozilla.org/en-US/docs/JavaScript/Reference/Global_Objects/Array/concat"
    //   },
    //   {
    //     "name": "every",
    //     "type": "fn(test: fn(elt: ?, i: number) -> bool, context?: ?) -> bool",
    //     "doc": "Tests whether all elements in the array pass the test implemented by the provided function.",
    //     "url": "https://developer.mozilla.org/en-US/docs/JavaScript/Reference/Global_Objects/Array/every"
    //   }]}

    entries.clear();

    JSON root(output);
    JSONItem completionsArr = root.toElement().namedObject("completions");
    for(int i = 0; i < completionsArr.arraySize(); ++i) {
        JSONItem item = completionsArr.arrayItem(i);
        wxString name = item.namedObject("name").toString();
        wxString doc = item.namedObject("doc").toString();
        wxString url = item.namedObject("url").toString();
        bool isKeyword = item.namedObject("isKeyword").toBool();
        int imgId;
        if(!isKeyword) {
            doc = this->PrepareDoc(doc, url);
            wxString type = item.namedObject("type").toString();
            wxString sig, ret;
            ProcessType(type, sig, ret, imgId);

            // Remove double quotes
            name.StartsWith("\"", &name);
            name.EndsWith("\"", &name);

            wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New(name /* + sig*/, imgId);
            entry->SetComment(doc);
            entries.push_back(entry);

        } else {
            imgId = 17; // keyword
            wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New(name, imgId);
            entries.push_back(entry);
        }
    }
}

void clTernServer::OnError(const wxString& why)
{
    m_workerThread->Stop();
    wxDELETE(m_workerThread);
    CL_ERROR("[WebTools] %s", why);
}

void clTernServer::OnTernWorkerThreadDone(const clTernWorkerThread::Reply& reply)
{
    m_workerThread->Stop();
    wxDELETE(m_workerThread);
    RecycleIfNeeded();

    m_entries.clear();
    CL_DEBUGS(reply.json);

    switch(reply.requestType) {
    case clTernWorkerThread::kFunctionTip:
        m_jsCCManager->OnFunctionTipReady(ProcessCalltip(reply.json), reply.filename);
        break;
    case clTernWorkerThread::kCodeCompletion:
        ProcessOutput(reply.json, m_entries);
        m_jsCCManager->OnCodeCompleteReady(m_entries, reply.filename);
        break;
    case clTernWorkerThread::kFindDefinition: {
        clTernDefinition loc;
        if(ProcessDefinitionOutput(reply.json, loc)) { m_jsCCManager->OnDefinitionFound(loc); }
    } break;
    case clTernWorkerThread::kReparse: {
        // TODO ??
    } break;
    case clTernWorkerThread::kReset:
        // TODO ??
        break;
    }
}

clCallTipPtr clTernServer::ProcessCalltip(const wxString& output)
{
    // Function calltip response:
    // ================================
    // {
    //   "type": "fn(f: fn(elt: ?, i: number), context?: ?)",
    //   "name": "Array.prototype.forEach",
    //   "exprName": "forEach",
    //   "url": "https://developer.mozilla.org/en-US/docs/JavaScript/Reference/Global_Objects/Array/forEach",
    //   "doc": "Executes a provided function once per array element.",
    //   "origin": "ecma5"
    // }
    TagEntryPtrVector_t tags;
    TagEntryPtr t(new TagEntry());
    JSON root(output);
    wxString type = root.toElement().namedObject("type").toString();
    int imgID;
    wxString sig, retValue;
    ProcessType(type, sig, retValue, imgID);
    if(sig.IsEmpty()) { return NULL; }
    t->SetSignature(sig);
    t->SetReturnValue(retValue);
    t->SetKind("function");
    t->SetFlags(TagEntry::Tag_No_Signature_Format);
    tags.push_back(t);
    return new clCallTip(tags);
}

JSONItem clTernServer::CreateLocation(wxStyledTextCtrl* ctrl, int pos)
{
    if(pos == wxNOT_FOUND) { pos = ctrl->GetCurrentPos(); }
    int lineNo = ctrl->LineFromPosition(pos);
    JSONItem loc = JSONItem::createObject("end");
    loc.addProperty("line", lineNo);

    // Pass the column
    int lineStartPos = ctrl->PositionFromLine(lineNo);
    pos = pos - lineStartPos;
    loc.addProperty("ch", pos);
    return loc;
}

bool clTernServer::PostFunctionTipRequest(IEditor* editor, int pos)
{
    // Sanity
    if(m_workerThread) return false;        // another request is in progress
    if(m_port == wxNOT_FOUND) return false; // don't know tern's port
    ++m_recycleCount;

    wxStyledTextCtrl* ctrl = editor->GetCtrl();

    // Write the modified buffer into a file
    // wxFileName tmpFileName = wxFileName::CreateTempFileName("tern");
    // if(!FileUtils::WriteFileContent(tmpFileName, ctrl->GetText())) return false;

    // Prepare the request
    JSON root(cJSON_Object);
    JSONItem query = JSONItem::createObject("query");
    root.toElement().append(query);
    query.addProperty("type", wxString("type"));
    query.addProperty("file", wxString("#0"));
    query.append(CreateLocation(ctrl, pos));

    // Creae the files array
    JSONItem files = CreateFilesArray(editor);
    root.toElement().append(files);

    clTernWorkerThread::Request* req = new clTernWorkerThread::Request;
    req->jsonRequest = root.toElement().FormatRawString();
    req->filename = editor->GetFileName().GetFullPath();
    req->type = clTernWorkerThread::kFunctionTip;

    // Create the worker thread and start the request
    m_workerThread = new clTernWorkerThread(this);
    m_workerThread->Start();
    m_workerThread->Add(req);
    return true;
}

JSONItem clTernServer::CreateFilesArray(IEditor* editor, bool forDelete)
{
    const wxString fileContent = editor->GetCtrl()->GetText();
    JSONItem files = JSONItem::createArray("files");

    JSONItem file = JSONItem::createObject();
    files.arrayAppend(file);

    wxString filename;
    if(!m_workingDirectory.IsEmpty()) {
        wxFileName fn(editor->GetFileName());
        fn.MakeRelativeTo(m_workingDirectory);
        filename = fn.GetFullPath();
    } else {
        filename = editor->GetFileName().GetFullName();
    }

    if(forDelete) {
        file.addProperty("type", wxString("delete"));
        file.addProperty("name", filename);

    } else {
        file.addProperty("type", wxString("full"));
        file.addProperty("name", filename);
        file.addProperty("text", fileContent);
    }
    return files;
}

bool clTernServer::LocateNodeJS(wxFileName& nodeJS)
{
    nodeJS = clNodeJS::Get().GetNode();
    return nodeJS.IsOk() && nodeJS.FileExists();
}

void clTernServer::ClearFatalErrorFlag() { m_fatalError = false; }

bool clTernServer::PostFindDefinitionRequest(IEditor* editor)
{
    // Sanity
    if(m_workerThread) return false;        // another request is in progress
    if(m_port == wxNOT_FOUND) return false; // don't know tern's port
    ++m_recycleCount;

    wxStyledTextCtrl* ctrl = editor->GetCtrl();

    // Prepare the request
    JSON root(cJSON_Object);
    JSONItem query = JSONItem::createObject("query");
    root.toElement().append(query);
    query.addProperty("type", wxString("definition"));
    query.addProperty("file", wxString("#0"));
    query.append(CreateLocation(ctrl));

    // Creae the files array
    JSONItem files = CreateFilesArray(editor);
    root.toElement().append(files);

    clTernWorkerThread::Request* req = new clTernWorkerThread::Request;
    req->jsonRequest = root.toElement().FormatRawString();
    req->filename = editor->GetFileName().GetFullPath();
    req->type = clTernWorkerThread::kFindDefinition;

    // Create the worker thread and start the request
    m_workerThread = new clTernWorkerThread(this);
    m_workerThread->Start();
    m_workerThread->Add(req);
    return true;
}

bool clTernServer::ProcessDefinitionOutput(const wxString& output, clTernDefinition& loc)
{
    JSON root(output);
    JSONItem json = root.toElement();

    if(json.hasNamedObject("file")) {
        wxFileName fn(json.namedObject("file").toString());
        if(!m_workingDirectory.IsEmpty()) { fn.MakeAbsolute(m_workingDirectory); }
        loc.file = fn.GetFullPath();
        loc.start = json.namedObject("start").toInt();
        loc.end = json.namedObject("end").toInt();
        return true;

    } else if(json.hasNamedObject("url")) {
        loc.url = json.namedObject("url").toString();
        return true;
    }
    return false;
}

bool clTernServer::PostResetCommand(bool forgetFiles)
{
    // Sanity
    if(m_workerThread) return false;        // another request is in progress
    if(m_port == wxNOT_FOUND) return false; // don't know tern's port
    ++m_recycleCount;

    // Prepare the request
    JSON root(cJSON_Object);
    JSONItem query = JSONItem::createObject("query");
    root.toElement().append(query);
    query.addProperty("type", wxString("reset"));
    if(forgetFiles) { query.addProperty("forgetFiles", true); }

    clTernWorkerThread::Request* req = new clTernWorkerThread::Request;
    req->jsonRequest = root.toElement().FormatRawString();
    req->type = clTernWorkerThread::kReset;

    // Create the worker thread and start the request
    m_workerThread = new clTernWorkerThread(this);
    m_workerThread->Start();
    m_workerThread->Add(req);
    return true;
}

bool clTernServer::PostReparseCommand(IEditor* editor)
{
    // Sanity
    if(!editor) return false;
    if(m_workerThread) return false;        // another request is in progress
    if(m_port == wxNOT_FOUND) return false; // don't know tern's port
    ++m_recycleCount;

    // Prepare the request
    JSON root(cJSON_Object);
    JSONItem files = CreateFilesArray(editor);
    root.toElement().append(files);

    clTernWorkerThread::Request* req = new clTernWorkerThread::Request;
    req->jsonRequest = root.toElement().FormatRawString();
    req->type = clTernWorkerThread::kReparse;

    // Create the worker thread and start the request
    m_workerThread = new clTernWorkerThread(this);
    m_workerThread->Start();
    m_workerThread->Add(req);
    return true;
}
