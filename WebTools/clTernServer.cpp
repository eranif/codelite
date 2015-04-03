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

BEGIN_EVENT_TABLE(clTernServer, wxEvtHandler)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_TERMINATED, clTernServer::OnTernTerminated)
EVT_COMMAND(wxID_ANY, wxEVT_PROC_DATA_READ, clTernServer::OnTernOutput)
END_EVENT_TABLE()

clTernServer::clTernServer(JSCodeCompletion* cc)
    : m_jsCCManager(cc)
    , m_tern(NULL)
    , m_port(16456)
    , m_goingDown(false)
    , m_workerThread(NULL)
    , m_fatalError(false)
{
}

clTernServer::~clTernServer() {}

void clTernServer::OnTernOutput(wxCommandEvent& event)
{
    ProcessEventData* ped = reinterpret_cast<ProcessEventData*>(event.GetClientData());
    PrintMessage(wxString::Format("Tern: %s\n", ped->GetData()));
    wxDELETE(ped);
}

void clTernServer::OnTernTerminated(wxCommandEvent& event)
{
    ProcessEventData* ped = reinterpret_cast<ProcessEventData*>(event.GetClientData());
    wxDELETE(ped);
    // Cleanup
    for(size_t i = 0; i < m_tempfiles.size(); ++i) {
        ::wxLogNull nolog;
        ::wxRemoveFile(m_tempfiles.Item(i));
    }
    m_tempfiles.Clear();
    if(m_goingDown) return;
    PrintMessage("Tern server terminated, will restart it\n");
    Start();
}

bool clTernServer::Start()
{
    if(m_fatalError) return false;
    WebToolsConfig conf;
    conf.Load();
    wxString workingDirectory;

    wxFileName ternFolder(clStandardPaths::Get().GetUserDataDir(), "");
    ternFolder.AppendDir("webtools");
    ternFolder.AppendDir("js");

#ifdef __WXGTK__
    wxFileName nodeJS("/usr/bin", "nodejs");
    if(!nodeJS.FileExists()) {
        m_fatalError = true;
        return false;
    }

#else
    // OSX and Windows
    wxFileName nodeJS(ternFolder.GetPath(), "node");
#ifdef __WXMSW__
    nodeJS.SetExt("exe");
#else
    // OSX
    nodeJS.SetExt("osx");

    // set permissions to 755
    nodeJS.SetPermissions(wxPOSIX_GROUP_READ | wxPOSIX_GROUP_EXECUTE | wxPOSIX_OTHERS_READ | wxPOSIX_OTHERS_EXECUTE |
                          wxPOSIX_USER_READ | wxPOSIX_USER_WRITE | wxPOSIX_USER_EXECUTE);
#endif
#endif
    wxString nodeExe = nodeJS.GetFullPath();
    ::WrapWithQuotes(nodeExe);

    wxString command;
    command << nodeExe  << " "
            << "bin" << wxFileName::GetPathSeparator() << "tern --persist --port " << GetPort();

    if(conf.HasJavaScriptFlag(WebToolsConfig::kJSEnableVerboseLogging)) {
        command << " --verbose";
    }

    // Create a .tern-project file
    // {
    //   "libs": [ "ecma5", "browser"],
    //   "plugins": {
    //     "node": {},
    //     "complete_strings": {}
    //   }
    // }
    {
        wxFileName ternConfig(nodeJS.GetPath(), ".tern-project");
        JSONRoot root(cJSON_Object);
        JSONElement libs = JSONElement::createArray("libs");
        root.toElement().append(libs);

        if(conf.HasJavaScriptFlag(WebToolsConfig::kJSLibraryBrowser)) {
            libs.arrayAppend("browser");
        }
        if(conf.HasJavaScriptFlag(WebToolsConfig::kJSLibraryChai)) {
            libs.arrayAppend("chai");
        }
        if(conf.HasJavaScriptFlag(WebToolsConfig::kJSLibraryEcma5)) {
            libs.arrayAppend("ecma5");
        }
        if(conf.HasJavaScriptFlag(WebToolsConfig::kJSLibraryEcma6)) {
            libs.arrayAppend("ecma6");
        }
        if(conf.HasJavaScriptFlag(WebToolsConfig::kJSLibraryJQuery)) {
            libs.arrayAppend("jquery");
        }
        if(conf.HasJavaScriptFlag(WebToolsConfig::kJSLibraryUnderscore)) {
            libs.arrayAppend("underscore");
        }
        JSONElement plugins = JSONElement::createObject("plugins");
        root.toElement().append(plugins);
        JSONElement node = JSONElement::createObject("node");
        plugins.append(node);
        JSONElement complete_strings = JSONElement::createObject("complete_strings");
        plugins.append(complete_strings);

        root.save(ternConfig);
    }

    PrintMessage(wxString() << "Starting " << command << "\n");
    m_tern = ::CreateAsyncProcess(this, command, IProcessCreateDefault, ternFolder.GetPath());
    if(!m_tern) {
        PrintMessage("Failed to start Tern server!");
        return false;
    }
    return true;
}

void clTernServer::Terminate()
{
    m_goingDown = true;
    wxDELETE(m_tern);
}

bool clTernServer::PostRequest(const wxString& request,
                               const wxFileName& filename,
                               const wxFileName& tmpFileName,
                               bool isFunctionCalltip)
{
    if(m_workerThread) return false; // another request is in progress

    m_workerThread = new clTernWorkerThread(this);
    m_workerThread->Start();

    clTernWorkerThread::Request* req = new clTernWorkerThread::Request;
    req->jsonRequest = request;
    req->filename = filename.GetFullPath();
    req->isFunctionTip = isFunctionCalltip;

    m_workerThread->Add(req);
    m_tempfiles.Add(tmpFileName.GetFullPath());
    return true;
}

void clTernServer::PrintMessage(const wxString& message)
{
    ::clGetManager()->AppendOutputTabText(kOutputTab_Output, wxString() << "[WebTools] " << message);
}

void clTernServer::RecycleIfNeeded(bool force)
{
    if(m_tern && ((m_tempfiles.size() > 500) || force)) {
        m_tern->Terminate();
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
            wxCodeCompletionBoxEntry::Ptr_t entry = wxCodeCompletionBoxEntry::New(name + sig, imgId);
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

    t->SetSignature(sig);
    t->SetReturnValue(retValue);
    t->SetKind("function");
    t->SetFlags(TagEntry::Tag_No_Signature_Format);
    tags.push_back(t);
    return new clCallTip(tags);
}
