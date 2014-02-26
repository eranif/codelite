#include "svn_default_command_handler.h"
#include "subversion2.h"
#include "subversion_view.h"
#include "event_notifier.h"

SvnDefaultCommandHandler::SvnDefaultCommandHandler(Subversion2 *plugin, int commandId, wxEvtHandler *owner)
    : SvnCommandHandler(plugin, commandId, owner)
{
}

SvnDefaultCommandHandler::~SvnDefaultCommandHandler()
{
}

void SvnDefaultCommandHandler::Process(const wxString &output)
{
    wxUnusedVar(output);

    // Reload any modified files
    EventNotifier::Get()->PostReloadExternallyModifiedEvent(false);

    // Refresh the SVN output page
    GetPlugin()->GetSvnView()->BuildTree();
}
