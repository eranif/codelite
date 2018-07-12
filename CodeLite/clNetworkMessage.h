#ifndef CLNETWORKMESSAGE_H
#define CLNETWORKMESSAGE_H

#include "codelite_exports.h"
#include <wx/string.h>
#include "wxStringHash.h"
#include "SocketAPI/clSocketBase.h"
#include "SocketAPI/clSocketClientAsync.h"

class WXDLLIMPEXP_CL clNetworkMessage
{
    std::unordered_map<wxString, wxString> m_values;

public:
    clNetworkMessage();
    clNetworkMessage(const wxString& str);
    virtual ~clNetworkMessage();

    wxString& operator[](const wxString& index)
    {
        if(m_values.count(index) == 0) {
            m_values[index] = wxString(); // add new entry
        }
        return m_values[index];
    }

    const wxString& operator[](const wxString& index) const
    {
        std::unordered_map<wxString, wxString>::const_iterator iter = m_values.find(index);
        if(iter == m_values.end()) {
            static wxString emptyString;
            return emptyString;
        }
        return iter->second;
    }
    
    /**
     * @brief serialize the message into a string
     */
    wxString ToString() const;
    
    /**
     * @brief send this message over the network
     * @throw clSocketException
     */
    void SendMessage(clSocketBase* socket);
    /**
     * @brief send this message over the network
     * @throw clSocketException
     */
    void SendMessage(clSocketClientAsync* socket);
};

#endif // CLNETWORKMESSAGE_H
