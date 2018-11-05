#ifndef NODEJSDEVTOOLSPROTOCOL_H
#define NODEJSDEVTOOLSPROTOCOL_H

#include <wx/arrstr.h>
class NodeJSDevToolsProtocol
{
    long message_id = 0;
public:
    NodeJSDevToolsProtocol();
    virtual ~NodeJSDevToolsProtocol();

    wxArrayString GetStartupCommands();
};

#endif // NODEJSDEVTOOLSPROTOCOL_H
