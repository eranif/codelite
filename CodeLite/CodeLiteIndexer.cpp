#include "CodeLiteIndexer.hpp"
#include "asyncprocess.h"
#include "clTempFile.hpp"
#include "cl_indexer_request.h"
#include "clindexerprotocol.h"
#include "ctags_manager.h"
#include "file_logger.h"
#include "named_pipe_client.h"
#include <sstream>
#include <vector>

#ifdef __WXMSW__
#define PIPE_NAME "\\\\.\\pipe\\codelite_indexer_%s"
#else
#include <sys/wait.h>
#define PIPE_NAME "/tmp/codelite/%s/codelite_indexer.sock"
#endif

namespace
{
wxArrayString vector_to_wx_array(const vector<wxString>& v)
{
    wxArrayString wx_arr;
    wx_arr.reserve(v.size());
    for(const wxString& str : v) {
        wx_arr.Add(str);
    }
    return wx_arr;
}
} // namespace

CodeLiteIndexer::CodeLiteIndexer()
{
    wxString uid;
    uid << wxGetProcessId();

#ifndef __WXMSW__
    wxFileName::Mkdir("/tmp/codelite/" + uid, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    string channel_path = get_channel_path();
    ::unlink(channel_path.c_str());
    ::remove(channel_path.c_str());
#endif
}

CodeLiteIndexer::~CodeLiteIndexer()
{
    stop();
#ifndef __WXMSW__
    ::waitpid(-1, NULL, WNOHANG);
#endif
}

void CodeLiteIndexer::set_exe_path(const wxFileName& exepath) { m_exepath = exepath; }

void CodeLiteIndexer::start()
{
#ifndef __WXMSW__
    ::waitpid(-1, NULL, WNOHANG);
#endif

    wxDELETE(m_process);
    wxString uid;
    uid << wxGetProcessId();

    vector<wxString> command = { m_exepath.GetFullPath(), uid, "--pid" };
    m_process = ::CreateAsyncProcess(nullptr, command, IProcessCreateWithHiddenConsole | IProcessNoRedirect);
    clDEBUG() << "Starting codelite-index:" << vector_to_wx_array(command) << endl;
}

bool CodeLiteIndexer::is_running() const { return m_process && m_process->IsAlive(); }

string CodeLiteIndexer::get_channel_path() const
{
    std::stringstream s;
    s << ::wxGetProcessId();

    char channel_name[1024];
    memset(channel_name, 0, sizeof(channel_name));

    sprintf(channel_name, PIPE_NAME, s.str().c_str());
    return channel_name;
}

void CodeLiteIndexer::buffer_to_tags(const wxString& content, wxString& tags) const
{
    clTempFile tmpfile("hpp");
    tmpfile.Write(content);
    source_to_tags(tmpfile.GetFileName(), tags);
}

void CodeLiteIndexer::source_to_tags(const wxFileName& source, wxString& tags) const
{
    auto channel_path = get_channel_path();
    clNamedPipeClient client(channel_path.c_str());

    // Build a request for the indexer
    clIndexerRequest req;
    // set the command
    req.setCmd(clIndexerRequest::CLI_PARSE);

    // prepare list of files to be parsed
    std::vector<std::string> files;
    files.push_back(source.GetFullPath().mb_str(wxConvUTF8).data());
    req.setFiles(files);

    // set ctags options to be used
    wxString ctagsCmd;
    ctagsCmd << TagsManagerST::Get()->GetCtagsOptions().ToString()
             << " --excmd=pattern --sort=no --fields=aKmSsnit --c-kinds=+p --c++-kinds=+p";
    req.setCtagOptions(ctagsCmd.mb_str(wxConvUTF8).data());

    // clDEBUG1() << "Sending CTAGS command:" << ctagsCmd << clEndl;
    // connect to the indexer
    if(!client.connect()) {
        clWARNING() << "Failed to connect to codelite_indexer:" << channel_path << endl;
        return;
    }

    // send the request
    if(!clIndexerProtocol::SendRequest(&client, req)) {
        clWARNING() << "Failed to send request to codelite_indexer:" << channel_path << endl;
        return;
    }

    // read the reply
    clIndexerReply reply;
    try {
        std::string errmsg;
        if(!clIndexerProtocol::ReadReply(&client, reply, errmsg)) {
            clWARNING() << "Failed to read indexer reply: " << (wxString() << errmsg) << endl;
            return;
        }
    } catch(std::bad_alloc& ex) {
        clWARNING() << "std::bad_alloc exception caught" << clEndl;
        tags.Clear();
        return;
    }

    // convert the data into wxString
    tags = wxString(reply.getTags().c_str(), wxConvUTF8);
    if(tags.empty()) {
        tags = wxString::From8BitData(reply.getTags().c_str());
    }
}

void CodeLiteIndexer::restart()
{
    stop();
    start();
}

void CodeLiteIndexer::stop() { wxDELETE(m_process); }
