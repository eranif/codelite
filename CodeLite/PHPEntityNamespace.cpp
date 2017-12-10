#include "PHPEntityNamespace.h"
#include <wx/tokenzr.h>
#include "PHPLookupTable.h"

PHPEntityNamespace::PHPEntityNamespace() {}

PHPEntityNamespace::~PHPEntityNamespace() {}

void PHPEntityNamespace::PrintStdout(int indent) const
{
    wxString indentString(' ', indent);
    wxPrintf("%sNamespace name: %s\n", indentString, GetFullName());

    PHPEntityBase::List_t::const_iterator iter = m_children.begin();
    for(; iter != m_children.end(); ++iter) {
        (*iter)->PrintStdout(indent + 4);
    }
}

void PHPEntityNamespace::Store(PHPLookupTable* lookup)
{
    try {
        // A namespace, unlike other PHP entities, can be defined in various files
        // and in multiple locations. This means, that by definition, there can be multiple entries
        // for the same namespace, however, since our relations in the database is ID based,
        // we try to locate the namespace in the DB before we attempt to insert it
        wxSQLite3Database& db = lookup->Database();
        {
            wxSQLite3Statement statement =
                db.PrepareStatement("SELECT * FROM SCOPE_TABLE WHERE FULLNAME=:FULLNAME LIMIT 1");
            statement.Bind(statement.GetParamIndex(":FULLNAME"), GetFullName());
            wxSQLite3ResultSet res = statement.ExecuteQuery();
            if(res.NextRow()) {
                // we have a match, update this item database ID to match
                // what we have found in the database
                PHPEntityNamespace ns;
                ns.FromResultSet(res);
                SetDbId(ns.GetDbId());
                return;
            }
        }

        // Get the 'parent' namespace part
        wxString parentPath = GetFullName().BeforeLast('\\');
        DoEnsureNamespacePathExists(db, parentPath);

        {
            wxSQLite3Statement statement = db.PrepareStatement(
                "INSERT INTO SCOPE_TABLE (ID, SCOPE_TYPE, SCOPE_ID, NAME, FULLNAME, LINE_NUMBER, FILE_NAME) "
                "VALUES (NULL, 0, -1, :NAME, :FULLNAME, :LINE_NUMBER, :FILE_NAME)");
            statement.Bind(statement.GetParamIndex(":NAME"), GetShortName());
            statement.Bind(statement.GetParamIndex(":FULLNAME"), GetFullName());
            statement.Bind(statement.GetParamIndex(":LINE_NUMBER"), GetLine());
            statement.Bind(statement.GetParamIndex(":FILE_NAME"), GetFilename().GetFullPath());
            statement.ExecuteUpdate();
            SetDbId(db.GetLastRowId());
        }
    } catch(wxSQLite3Exception& exc) {
        wxUnusedVar(exc);
    }
}

void PHPEntityNamespace::FromResultSet(wxSQLite3ResultSet& res)
{
    SetDbId(res.GetInt64("ID"));
    SetFullName(res.GetString("FULLNAME"));
    SetShortName(res.GetString("NAME"));
    SetFilename(res.GetString("FILE_NAME"));
    SetLine(res.GetInt("LINE_NUMBER"));
    SetFlags(0);
}

wxString PHPEntityNamespace::Type() const { return GetFullName(); }
bool PHPEntityNamespace::Is(eEntityType type) const { return type == kEntityTypeNamespace; }
wxString PHPEntityNamespace::GetDisplayName() const { return GetFullName(); }
wxString PHPEntityNamespace::FormatPhpDoc(const CommentConfigData& data) const
{
    wxString doc;
    doc << data.GetCommentBlockPrefix() << "\n"
        << " * @brief \n";
    doc << " */";
    return doc;
}

void PHPEntityNamespace::DoEnsureNamespacePathExists(wxSQLite3Database& db, const wxString& path)
{
    wxArrayString paths = ::wxStringTokenize(path, "\\", wxTOKEN_STRTOK);
    if(paths.IsEmpty()) return;

    wxString currentPath;
    try {
        for(size_t i = 0; i < paths.GetCount(); ++i) {
            if(!currentPath.EndsWith("\\")) {
                currentPath << "\\";
            }
            currentPath << paths.Item(i);
            wxSQLite3Statement statement = db.PrepareStatement(
                "INSERT OR IGNORE INTO SCOPE_TABLE (ID, SCOPE_TYPE, SCOPE_ID, NAME, FULLNAME, LINE_NUMBER, FILE_NAME) "
                "VALUES (NULL, 0, -1, :NAME, :FULLNAME, :LINE_NUMBER, :FILE_NAME)");
            statement.Bind(statement.GetParamIndex(":NAME"), paths.Item(i));
            statement.Bind(statement.GetParamIndex(":FULLNAME"), currentPath);
            statement.Bind(statement.GetParamIndex(":LINE_NUMBER"), GetLine());
            statement.Bind(statement.GetParamIndex(":FILE_NAME"), GetFilename().GetFullPath());
            statement.ExecuteUpdate();
            // SetDbId(db.GetLastRowId());
        }

    } catch(wxSQLite3Exception& exc) {
        wxUnusedVar(exc);
    }
}

wxString PHPEntityNamespace::GetParentNamespace() const
{
    if(GetFullName() == "\\") {
        // this is the global namespace
        return "\\";
    }
    wxString parentPath = GetFullName().BeforeLast('\\');
    if(parentPath.IsEmpty()) return "\\";
    return parentPath;
}

wxString PHPEntityNamespace::BuildNamespace(const wxString& part1, const wxString& part2)
{
    wxString ns;
    ns << part1 << "\\" << part2;
    while(ns.Replace("\\\\", "\\")) {
    }
    return ns;
}
