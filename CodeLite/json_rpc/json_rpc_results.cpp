#include "json_rpc/json_rpc_results.h"

void json_rpc::ResultString::FromJSON(const JSONItem& json) { m_text = json.toString(); }
void json_rpc::ResultNumber::FromJSON(const JSONItem& json) { m_number = json.toInt(m_number); }
void json_rpc::ResultBoolean::FromJSON(const JSONItem& json) { m_value = json.toBool(m_value); }
