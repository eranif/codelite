#include "clTernServer.h"
#include "processreaderthread.h"
#include "cl_standard_paths.h"
#include "globals.h"
#include "imanager.h"
#include "asyncprocess.h"
#include "SocketAPI/clSocketClient.h"
#include "file_logger.h"
#include "wxCodeCompletionBoxEntry.h"
#include "clTernWorkerThread.h"
#include "JSCodeCompletion.h"
#include "WebToolsConfig.h"
#include "entry.h"
#include <wx/regex.h>
#include <wx/log.h>
#include "fileutils.h"
#include "ieditor.h"
#include <wx/stc/stc.h>
#include <wx/msgdlg.h>

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
}

clTernServer::~clTernServer() {}

void clTernServer::OnTernOutput(clProcessEvent& event)
{
    static wxRegEx rePort("Listening on port ([0-9]+)");
    if(rePort.IsValid() && rePort.Matches(event.GetOutput())) {
        wxString strPort = rePort.GetMatch(event.GetOutput(), 1);
        strPort.ToCLong(&m_port);
    }
    PrintMessage(event.GetOutput());
}

void clTernServer::OnTernTerminated(clProcessEvent& event)
{
    wxDELETE(m_tern);
    if(m_goingDown || !m_jsCCManager->IsEnabled()) {
        return;
    }
    PrintMessage("Tern server terminated, will restart it\n");
    Start(m_workingDirectory);
}

bool clTernServer::Start(const wxString& workingDirectory)
{
    if(m_fatalError) return false;
    if(!m_jsCCManager->IsEnabled()) return true;

    m_workingDirectory = workingDirectory;
    WebToolsConfig conf;
    conf.Load();

    wxFileName ternFolder(clStandardPaths::Get().GetUserDataDir(), "");
    ternFolder.AppendDir("webtools");
    ternFolder.AppendDir("js");

    wxFileName nodeJS;
    if(!LocateNodeJS(nodeJS)) {
        m_fatalError = true;
        return false;
    }

#ifdef __WXMAC__
    // set permissions to 755
    nodeJS.SetPermissions(wxPOSIX_GROUP_READ | wxPOSIX_GROUP_EXECUTE | wxPOSIX_OTHERS_READ | wxPOSIX_OTHERS_EXECUTE |
                          wxPOSIX_USER_READ | wxPOSIX_USER_WRITE | wxPOSIX_USER_EXECUTE);
#endif

    wxString nodeExe = nodeJS.GetFullPath();
    ::WrapWithQuotes(nodeExe);

    wxFileName ternScript = ternFolder;
    ternScript.AppendDir("bin");
    ternScript.SetFullName("tern");
    wxString ternScriptString = ternScript.GetFullPath();
    ::WrapWithQuotes(ternScriptString);

    wxString command;
    command << nodeExe << " " << ternScriptString << " --persistent ";

    if(conf.HasJavaScriptFlag(WebToolsConfig::kJSEnableVerboseLogging)) {
        command << " --verbose";
    }

    // Create a .tern-project file
    if(m_workingDirectory.IsEmpty()) {
        m_workingDirectory = clStandardPaths::Get().GetUserDataDir();
    }

    wxFileName ternConfig(m_workingDirectory, ".tern-project");
    wxString content = conf.GetTernProjectFile();
    if(!FileUtils::WriteFileContent(ternConfig, content)) {
        ::wxMessageBox(_("Could not write tern project file: ") + ternConfig.GetFullPath(),
                       "CodeLite",
                       wxICON_ERROR | wxOK | wxCENTER);
        PrintMessage("Could not write tern project file: " + ternConfig.GetFullPath());
        m_fatalError = true;
        return false;
    }

    PrintMessage(wxString() << "Starting " << command << "\n");
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
    if(m_tern) {
        m_tern->Terminate();
    }
    wxDELETE(m_tern);

    // Stop the worker thread
    if(m_workerThread) {
        m_workerThread->Stop();
    }
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
    JSONRoot root(cJSON_Object);
    JSONElement query = JSONElement::createObject("query");
    root.toElement().append(query);
    query.addProperty("type", wxString("completions"));
    query.addProperty("file", wxString("#0"));
    query.append(CreateLocation(ctrl));
    query.addProperty("docs", true);
    query.addProperty("urls", true);
    query.addProperty("includeKeywords", true);
    query.addProperty("types", true);

    JSONElement files = CreateFilesArray(ctrl);
    root.toElement().append(files);

    clTernWorkerThread::Request* req = new clTernWorkerThread::Request;
    req->jsonRequest = root.toElement().FormatRawString();
    req->filename = editor->GetFileName().GetFullPath();
    req->isFunctionTip = false;

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
    CL_DEBUGS(msg);
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

    if(!curline.IsEmpty()) {
        content << curline << "\n";
    }
    if(!url.IsEmpty()) {
        content << "@link " << url;
    }
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

    JSONRoot root(output);
    JSONElement completionsArr = root.toElement().namedObject("completions");
    for(int i = 0; i < completionsArr.arraySize(); ++i) {
        JSONElement item = completionsArr.arrayItem(i);
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
    if(reply.isFunctionTip) {
        m_jsCCManager->OnFunctionTipReady(ProcessCalltip(reply.json), reply.filename);
    } else {
        ProcessOutput(reply.json, m_entries);
        m_jsCCManager->OnCodeCompleteReady(m_entries, reply.filename);
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
    JSONRoot root(output);
    wxString type = root.toElement().namedObject("type").toString();
    int imgID;
    wxString sig, retValue;
    ProcessType(type, sig, retValue, imgID);
    if(sig.IsEmpty()) {
        return NULL;
    }
    t->SetSignature(sig);
    t->SetReturnValue(retValue);
    t->SetKind("function");
    t->SetFlags(TagEntry::Tag_No_Signature_Format);
    tags.push_back(t);
    return new clCallTip(tags);
}

JSONElement clTernServer::CreateLocation(wxStyledTextCtrl* ctrl, int pos)
{
    if(pos == wxNOT_FOUND) {
        pos = ctrl->GetCurrentPos();
    }
    int lineNo = ctrl->LineFromPosition(pos);
    JSONElement loc = JSONElement::createObject("end");
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
    JSONRoot root(cJSON_Object);
    JSONElement query = JSONElement::createObject("query");
    root.toElement().append(query);
    query.addProperty("type", wxString("type"));
    query.addProperty("file", wxString("#0"));
    query.append(CreateLocation(ctrl, pos));

    // Creae the files array
    JSONElement files = CreateFilesArray(ctrl);
    root.toElement().append(files);

    clTernWorkerThread::Request* req = new clTernWorkerThread::Request;
    req->jsonRequest = root.toElement().FormatRawString();
    req->filename = editor->GetFileName().GetFullPath();
    req->isFunctionTip = true;

    // Create the worker thread and start the request
    m_workerThread = new clTernWorkerThread(this);
    m_workerThread->Start();
    m_workerThread->Add(req);
    return true;
}

JSONElement clTernServer::CreateFilesArray(wxStyledTextCtrl* ctrl)
{
    const wxString fileContent = ctrl->GetText();
    JSONElement files = JSONElement::createArray("files");

    JSONElement file = JSONElement::createObject();
    files.arrayAppend(file);

    file.addProperty("type", wxString("full"));
    file.addProperty("name", wxString("[doc]"));
    file.addProperty("text", fileContent);
    return files;
}

bool clTernServer::LocateNodeJS(wxFileName& nodeJS)
{
#ifdef __WXGTK__
    // Check that NodeJS is installed
    wxFileName fn("/usr/bin", "nodejs");
    if(!fn.FileExists()) {
        fn.SetFullName("node"); // On OpenSUSE, nodejs is called 'node'
        if(!fn.FileExists()) {
            return false;
        }
    }
    nodeJS = fn;
    return true;
#elif defined(__WXMSW__)
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "");
    fn.AppendDir("webtools");
    fn.AppendDir("js");
    fn.SetFullName("node.exe");
    nodeJS = fn;
    return true;
#else // OSX
    wxFileName fn(clStandardPaths::Get().GetUserDataDir(), "");
    fn.AppendDir("webtools");
    fn.AppendDir("js");
    fn.SetFullName("node.osx");
    nodeJS = fn;
    return true;
#endif
}

void clTernServer::ClearFatalErrorFlag()
{
    m_fatalError = false;
}
