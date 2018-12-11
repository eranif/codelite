#ifndef NODEDEBUGGERTOOLTIP_H
#define NODEDEBUGGERTOOLTIP_H

#include "clResizableTooltip.h" // Base class: clResizableTooltip
#include "cl_command_event.h"
#include "nSerializableObject.h"
#include "wxStringHash.h"
#include <unordered_map>

class NodeDebuggerTooltip : public clResizableTooltip
{
    std::unordered_map<wxString, wxTreeItemId> m_pendingItems;

protected:
    void OnItemExpanding(wxTreeEvent& event);
    wxString GetObjectId(const wxTreeItemId& item) const;
    void OnObjectProperties(clDebugEvent& event);

public:
    NodeDebuggerTooltip(wxWindow* parent);
    virtual ~NodeDebuggerTooltip();
    void Show(nSerializableObject::Ptr_t RemoteObject);
};

#endif // NODEDEBUGGERTOOLTIP_H
