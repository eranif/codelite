#include "Channel.hpp"
#include "ProtocolHandler.hpp"
#include "cl_standard_paths.h"
#include "ctags_manager.h"
#include "file_logger.h"
#include <iostream>
#include <stdio.h>
#include <unordered_map>
#include <wx/cmdline.h>
#include <wx/filename.h>
#include <wx/init.h>
#include <wx/stdpaths.h>

namespace
{
unordered_map<wxString, ProtocolHandler::CallbackFunc> function_table = {
    { "initialize", &ProtocolHandler::on_initialize },
    { "initialized", &ProtocolHandler::on_initialized },
    { "textDocument/didOpen", &ProtocolHandler::on_did_open },
    { "textDocument/didChange", &ProtocolHandler::on_did_change },
    { "textDocument/completion", &ProtocolHandler::on_completion },
    { "textDocument/didClose", &ProtocolHandler::on_did_close },
    { "textDocument/didSave", &ProtocolHandler::on_did_save },
    { "textDocument/semanticTokens/full", &ProtocolHandler::on_semantic_tokens },
    { "textDocument/signatureHelp", &ProtocolHandler::on_document_signature_help },
    { "textDocument/definition", &ProtocolHandler::on_definition },
    { "textDocument/hover", &ProtocolHandler::on_hover },
    { "textDocument/documentSymbol", &ProtocolHandler::on_document_symbol },
    { "textDocument/documentSymbol", &ProtocolHandler::on_document_symbol },
};
}

/// A wrapper around codelite_indexer that implements
/// the Language Server Protocol
int main(int argc, char** argv)
{
    wxInitializer initializer(argc, argv);
    wxFileName logdir(clStandardPaths::Get().GetUserDataDir(), wxEmptyString);
    logdir.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    wxCmdLineParser parser(argc, argv);
    parser.AddOption("p", "port", "Port number", wxCMD_LINE_VAL_NUMBER);
    parser.AddOption("h", "host", "Hostname");
    parser.AddLongOption("log-level", "Log level, one of: ERR, WARN, DBG, TRACE");
    parser.Parse();

    long port = 38478;
    wxString host = "127.0.0.1";
    wxString log_level_str = "ERR";
    parser.Found("port", &port);
    parser.Found("host", &host);
    parser.Found("log-level", &log_level_str);

    int log_level = FileLogger::GetVerbosityAsNumber(log_level_str);
    FileLogger::OpenLog("ctagsd.log", log_level);

    try {
        Channel channel;
        ProtocolHandler protocol_handler;
        clSYSTEM() << "Started main loop" << endl;
        channel.open(host, port);

        wxString message;
        while(true) {
            auto msg = channel.read_message();
            if(!msg) {
                break;
            }
            auto json = msg->toElement();
            wxString method = json["method"].toString();
            if(function_table.count(method) == 0) {
                clDEBUG1() << "Received unsupported method:" << method << endl;
                protocol_handler.on_unsupported_message(move(msg), channel);
            } else {
                auto& cb = function_table[method];
                (protocol_handler.*cb)(move(msg), channel);
            }
        }

    } catch(clSocketException& e) {
        clERROR() << "Uncaught exception:" << e.what() << endl;
        exit(1);
    }

    // Free resources allocated by the tags manager
    TagsManagerST::Free();
    return 0;
}
