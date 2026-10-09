#include "CodeActionResolveRequest.hpp"

#include "LSP/LSPEvent.h"
#include "LSP/ResponseError.h"
#include "event_notifier.h"

#include <wx/msgdlg.h>

LSP::CodeActionResolveRequest::CodeActionResolveRequest(const wxString& languageServerName,
                                                        const wxString& filepath,
                                                        const LSP::CodeAction& codeAction)
    : m_languageServerName(languageServerName)
    , m_filepath(filepath)
{
    SetMethod("codeAction/resolve");
    m_params.reset(new CodeActionResolveParams(codeAction.GetJSON()));
    LSP_DEBUG() << wxString::FromUTF8(ToJSON().dump(2)) << endl;
}

std::optional<LSPEvent> LSP::CodeActionResolveRequest::OnResponse(const LSP::ResponseMessage& response,
                                                                  [[maybe_unused]] wxEvtHandler* owner)
{
    LSP_DEBUG() << "LSP::CodeActionResolveRequest::OnResponse()" << endl;
    LSP_DEBUG() << response.ToString() << endl;
    auto result = response.Get("result");
    if (!result.isObject()) {
        LSP_WARNING() << "codeAction/resolve result is expected to be of type object" << endl;
        return std::nullopt;
    }

    LSP::CodeAction action;
    action.FromJSON(result);

    LSPEvent event{wxEVT_LSP_CODE_ACTION_RESOLVED};
    event.GetCodeActions().push_back(action);
    event.SetFileName(m_filepath);
    EventNotifier::Get()->AddPendingEvent(event);
    return event;
}

void LSP::CodeActionResolveRequest::HandleError(const LSP::ResponseMessage& response,
                                                [[maybe_unused]] wxEvtHandler* owner)
{
    LSP::ResponseError errMsg(response.ToString());
    wxMessageBox(wxString::Format(_("Error from language server %s:\n%s"), m_languageServerName, errMsg.GetMessage()),
                 m_languageServerName,
                 wxICON_ERROR | wxCENTER);
}
