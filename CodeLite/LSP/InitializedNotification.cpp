#include "InitializedNotification.hpp"

namespace LSP
{
struct InitializedParams : public Params {
    JSONItem ToJSON(const wxString& name, IPathConverter::Ptr_t pathConverter) const override
    {
        wxUnusedVar(name);
        wxUnusedVar(pathConverter);
        return JSONItem::createObject(name);
    }

    void FromJSON(const JSONItem& json, IPathConverter::Ptr_t pathConverter) override
    {
        wxUnusedVar(json);
        wxUnusedVar(pathConverter);
    };
};

InitializedNotification::InitializedNotification()
{
    SetMethod("initialized");
    m_params.reset(new InitializedParams());
}

InitializedNotification::~InitializedNotification() {}

} // namespace LSP