#ifndef WXCNETWORKCOMMAND_H
#define WXCNETWORKCOMMAND_H

#include "wxcLib/wxcEnums.h"
#include "wxcLib/json_node.h"

class wxcNetworkCommand
{
    wxCrafter::eCommandType m_commandType;
    wxString                m_filename;

public:
    wxcNetworkCommand();
    wxcNetworkCommand(const wxString& json);
    virtual ~wxcNetworkCommand();
    
    void SetCommandType(const wxCrafter::eCommandType& commandType) {
        this->m_commandType = commandType;
    }
    void SetFilename(const wxString& filename) {
        this->m_filename = filename;
    }
    const wxCrafter::eCommandType& GetCommandType() const {
        return m_commandType;
    }
    const wxString& GetFilename() const {
        return m_filename;
    }

    // Serialization API
    void FromJSON(const JSONElement& json);
    JSONElement ToJSON() const;
};

#endif // WXCNETWORKCOMMAND_H
