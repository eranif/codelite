#include "php_func_arg.h"
#include "php_doc_comment.h"

PHPFuncArg::PHPFuncArg()
    : dbId(-1)
    , entryId(-1)
    , lineNumber(php_lineno)
    , attributes(PHP_Access_Public)
{
}

PHPFuncArg::PHPFuncArg(wxSQLite3ResultSet& res)
{
    dbId       = res.GetInt(0);
    entryId    = res.GetInt(1);
    name       = res.GetString(2);
    type       = res.GetString(3);
    lineNumber = res.GetInt(4);
    attributes = res.GetInt(5);
    filename   = res.GetString(6);
    index      = res.GetInt(7);
}

void PHPFuncArg::save(wxSQLite3Database* db, int entryId)
{
    try {
        wxSQLite3Statement statement = db->PrepareStatement("REPLACE INTO FUNC_ARGS (ID, ENTRY_ID, NAME, TYPE, LINE_NUMBER, ATTRIBUTES, FILE_NAME, ARG_INDEX) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?)");
        statement.Bind(1,  entryId);
        statement.Bind(2,  getName());
        statement.Bind(3,  getType());
        statement.Bind(4,  getLineNumber());
        statement.Bind(5,  (wxLongLong)getAttributes());
        statement.Bind(6,  getFilename());
        statement.Bind(7,  (wxLongLong)getIndex());
        statement.ExecuteUpdate();
    } catch (wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
}

PHPFuncArg::Vector_t PHPFuncArg::loadByEntryId(wxSQLite3Database* db, long entryDbId)
{
    PHPFuncArg::Vector_t args;
    try {
        wxSQLite3Statement statement = db->PrepareStatement("SELECT * from FUNC_ARGS where ENTRY_ID = ?");
        statement.Bind(1, (int)entryDbId);
        wxSQLite3ResultSet res = statement.ExecuteQuery();
        while ( res.NextRow() ) {
            PHPFuncArg arg(res);
            args.push_back( arg );
        }
    } catch (wxSQLite3Exception& e) {
        wxUnusedVar(e);
    }
    return args;
}

wxString PHPFuncArg::getType(const wxString& doc) const
{
    if ( !doc.IsEmpty() ) {
        PHPDocComment d(doc);
        wxString typeFromComment = d.getParam(getIndex());
        if ( !typeFromComment.IsEmpty() ) {
            return typeFromComment;
        }
    }
    return type;
}
