#include "PHPEntityFunctionAlias.h"

#include "PHPEntityFunction.h"
#include "PHPLookupTable.h"
#include "file_logger.h"

PHPEntityFunctionAlias::PHPEntityFunctionAlias() {}

PHPEntityFunctionAlias::~PHPEntityFunctionAlias() {}

bool PHPEntityFunctionAlias::Is(eEntityType type) const { return type == kEntityTypeFunctionAlias; }

void PHPEntityFunctionAlias::FromResultSet(wxSQLite3ResultSet& res)
{
    SetDbId(res.GetInt("ID"));
    SetShortName(res.GetString("NAME"));
    SetRealname(res.GetString("REALNAME"));
    SetLine(res.GetInt("LINE_NUMBER"));
    SetFilename(res.GetString("FILE_NAME"));
    SetFullName(res.GetString("FULLNAME"));
    SetScope(res.GetString("SCOPE"));
}

void PHPEntityFunctionAlias::Store(PHPLookupTable* lookup)
{
    try {
        wxSQLite3Database& db = lookup->Database();
        wxSQLite3Statement statement = db.PrepareStatement(
            "INSERT OR REPLACE INTO FUNCTION_ALIAS_TABLE VALUES(NULL, :SCOPE_ID, :NAME, :REALNAME, :FULLNAME, :SCOPE, "
            ":LINE_NUMBER, :FILE_NAME)");
        statement.Bind(statement.GetParamIndex(":SCOPE_ID"), Parent()->GetDbId());
        statement.Bind(statement.GetParamIndex(":NAME"), GetShortName());
        statement.Bind(statement.GetParamIndex(":REALNAME"), GetRealname());
        statement.Bind(statement.GetParamIndex(":FULLNAME"), GetFullName());
        statement.Bind(statement.GetParamIndex(":SCOPE"), GetScope());
        statement.Bind(statement.GetParamIndex(":LINE_NUMBER"), GetLine());
        statement.Bind(statement.GetParamIndex(":FILE_NAME"), GetFilename().GetFullPath());
        statement.ExecuteUpdate();
        SetDbId(db.GetLastRowId());

    } catch(wxSQLite3Exception& exc) {
        clWARNING() << "PHPEntityFunctionAlias::Store:" << exc.GetMessage() << endl;
    }
}

wxString PHPEntityFunctionAlias::FormatPhpDoc(const CommentConfigData& data) const
{
    if(m_func) {
        return m_func->FormatPhpDoc(data);
    }
    return "";
}

wxString PHPEntityFunctionAlias::GetDisplayName() const { return GetShortName(); }

void PHPEntityFunctionAlias::PrintStdout(int indent) const { wxUnusedVar(indent); }

wxString PHPEntityFunctionAlias::Type() const
{
    if(m_func) {
        return m_func->Type();
    }
    return "";
}

void PHPEntityFunctionAlias::FromJSON(const JSONItem& json)
{
    BaseFromJSON(json);
    m_realname = json.namedObject("realName").toString();
    m_scope = json.namedObject("scope").toString();
    if(json.hasNamedObject("func")) {
        JSONItem func = json.namedObject("func");
        m_func.Reset(new PHPEntityFunction());
        m_func->FromJSON(func);
    }
}

JSONItem PHPEntityFunctionAlias::ToJSON() const
{
    JSONItem json = BaseToJSON("a");
    json.addProperty("realName", m_realname);
    json.addProperty("scope", m_scope);
    if(m_func) {
        JSONItem func = m_func->ToJSON();
        json.addProperty("func", func);
    }
    return json;
}
