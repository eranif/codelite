#include "InitializedNotification.hpp"

namespace LSP
{
struct InitializedParams : public Params {
    JSONItem ToJSON(const wxString& name) const override
    {
        wxUnusedVar(name);
        return JSONItem::createObject(name);
    }

    void FromJSON(const JSONItem& json) override { wxUnusedVar(json); };
};

InitializedNotification::InitializedNotification()
{
    SetMethod("initialized");
    m_params.reset(new InitializedParams());
}

InitializedNotification::~InitializedNotification() {}

} // namespace LSP
