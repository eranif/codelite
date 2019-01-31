#include "wxcNetworkThread.h"
#include "SocketAPI/clSocketBase.h"
#include "SocketAPI/clSocketServer.h"
#include "allocator_mgr.h"
#include "wxcEvent.h"
#include "wxcLib/wxcLibAPI.h"
#include <event_notifier.h>
#include <json_node.h>

wxcNetworkThread::wxcNetworkThread()
    : wxThread(wxTHREAD_JOINABLE)
{
}

wxcNetworkThread::~wxcNetworkThread() {}

void* wxcNetworkThread::Entry()
{
    clSocketServer server;
    try {
        wxString connectionString;
        connectionString << "tcp://127.0.0.1" << WXC_PORT_NUMBER_INT;
        server.Start(connectionString);

        clSocketBase::Ptr_t client;
        while(!TestDestroy() && !client) {
            client = server.WaitForNewConnection(1);
        }

        if(!client) {
            // Delete was called, return NULL
            return NULL;
        }

        // Dont close the connection on exit
        client->SetCloseOnExit(false);

        // send the connection to the main thread as well
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_CONN_ESTASBLISHED);
        event.SetInt(client->GetSocket());
        EventNotifier::Get()->AddPendingEvent(event);

        // Main Loop
        while(!TestDestroy()) {
            wxString message;
            if(client->ReadMessage(message, 1) == clSocketBase::kSuccess) {
                // Got a message
                ProcessMessage(message);
            }
        }

    } catch(clSocketException& e) {
        // something bad had happened
        return NULL;
    }
    return NULL;
}

void wxcNetworkThread::ProcessMessage(const wxString& message)
{
    wxcNetworkCommand command(message);
    switch(command.GetCommandType()) {
    case wxCrafter::kCommandTypeShowDesigner: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_SHOW_DESIGNER);
        EventNotifier::Get()->AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeExit: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_EXIT);
        EventNotifier::Get()->AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeLoadFile: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_LOAD_FILE);
        event.SetFileName(command.GetFilename());
        EventNotifier::Get()->AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeGenerateCode: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_CONN_GEN_CODE);
        event.SetFileName(command.GetFilename());
        EventNotifier::Get()->AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeNewDialog: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_NEW_FORM);
        event.SetFileName(command.GetFilename());
        event.SetFormId(ID_WXDIALOG);
        EventNotifier::Get()->AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeNewFrame: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_NEW_FORM);
        event.SetFileName(command.GetFilename());
        event.SetFormId(ID_WXFRAME);
        EventNotifier::Get()->AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeNewPanel: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_NEW_FORM);
        event.SetFileName(command.GetFilename());
        event.SetFormId(ID_WXPANEL_TOPLEVEL);
        EventNotifier::Get()->AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeNewPopupWindow: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_NEW_FORM);
        event.SetFileName(command.GetFilename());
        event.SetFormId(ID_WXPOPUPWINDOW);
        EventNotifier::Get()->AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeNewWizard: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_NEW_FORM);
        event.SetFileName(command.GetFilename());
        event.SetFormId(ID_WXWIZARD);
        EventNotifier::Get()->AddPendingEvent(event);
    } break;
    case wxCrafter::kCommandTypeNewImageList: {
        wxcNetworkEvent event(wxEVT_NETWORK_COMMAND_NEW_FORM);
        event.SetFileName(command.GetFilename());
        event.SetFormId(ID_WXIMAGELIST);
        EventNotifier::Get()->AddPendingEvent(event);
    } break;
    default:
        break;
    }
}
