#ifndef GOTODEFINITIONREQUEST_H
#define GOTODEFINITIONREQUEST_H

#include "RequestMessage.h" // Base class: json_rpc::RequestMessage
#include <wx/filename.h>
#include "basic_types.h"

namespace json_rpc
{
class WXDLLIMPEXP_CL GotoDefinitionRequest : public json_rpc::RequestMessage
{
public:
    GotoDefinitionRequest(const wxFileName& filename, size_t line, size_t column);
    virtual ~GotoDefinitionRequest();
};
};     // namespace json_rpc
#endif // GOTODEFINITIONREQUEST_H
