#include "PHPEntityKeyword.h"

PHPEntityKeyword::PHPEntityKeyword() {}

PHPEntityKeyword::~PHPEntityKeyword() {}

wxString PHPEntityKeyword::FormatPhpDoc(const CommentConfigData& data) const { return ""; }
void PHPEntityKeyword::FromResultSet(wxSQLite3ResultSet& res) {}
wxString PHPEntityKeyword::GetDisplayName() const { return m_shortName; }
bool PHPEntityKeyword::Is(eEntityType type) const { return type == kEntityTypeKeyword; }
void PHPEntityKeyword::PrintStdout(int indent) const { wxUnusedVar(indent); }
void PHPEntityKeyword::Store(PHPLookupTable* lookup) { wxUnusedVar(lookup); }
wxString PHPEntityKeyword::Type() const { return ""; }

void PHPEntityKeyword::FromJSON(const JSONItem& json) { BaseFromJSON(json); }

JSONItem PHPEntityKeyword::ToJSON() const { return BaseToJSON("k"); }
