#ifndef LSP_REQUEST_H
#define LSP_REQUEST_H

#include "LSP/MessageWithParams.h"

namespace LSP
{

class Request : public LSP::MessageWithParams
{
    int m_id = wxNOT_FOUND;

public:
    Request();
    virtual ~Request();

    void SetId(int id) { this->m_id = id; }
    int GetId() const { return m_id; }

    virtual JSONItem ToJSON(const wxString& name) const;
    virtual void FromJSON(const JSONItem& json);
};

} // namespace LSP

#endif // REQUEST_H
