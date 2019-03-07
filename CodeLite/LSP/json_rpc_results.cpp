#include "LSP/json_rpc_results.h"

void LSP::ResultString::FromJSON(const JSONItem& json) { m_text = json.toString(); }
void LSP::ResultNumber::FromJSON(const JSONItem& json) { m_number = json.toInt(m_number); }
void LSP::ResultBoolean::FromJSON(const JSONItem& json) { m_value = json.toBool(m_value); }
