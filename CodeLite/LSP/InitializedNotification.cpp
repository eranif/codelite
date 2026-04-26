#include "InitializedNotification.hpp"

namespace LSP
{
struct InitializedParams : public Params {
    JSONItem ToJSON() const override { return nlohmann::json::object(); }

    void FromJSON(const JSONItem& json) override { wxUnusedVar(json); };
};

InitializedNotification::InitializedNotification()
{
    SetMethod("initialized");
    m_params.reset(new InitializedParams());
}

} // namespace LSP
