#include "php_entry.h"
#include "php_parser_api.h" // PHPGlobals
#include "php_doc_comment.h"
#include <algorithm>
#include <wx/tokenzr.h>
#include <editor_config.h>
#include <commentconfigdata.h>
#include "php_storage.h"

extern PHPGlobals PHPParser;

struct _SAscendingSortArgs {
    bool operator()(const PHPFuncArg &a, const PHPFuncArg &b) {
        return (b.getIndex() > a.getIndex());
    }
};

static wxString EscapeString( const wxString &name )
{
    wxString tmpName(name);
    tmpName.Replace(wxT("_"), wxT("^_"));
    tmpName.Trim().Trim(false);
    return tmpName;
}

PHPEntry::PHPEntry(const wxString& classname)
    : dbId(-1)
    , parentDbId(-1)
    , kind(PHP_Kind_Class)
    , name(classname)
    , attributes(PHP_Access_Public)
    , lineNumber(-1)
    , terminatingOperator(0)
    , prefixOperator(0)
{
}

PHPEntry::PHPEntry()
    : dbId(-1)
    , parentDbId(-1)
    , kind(PHP_Kind_Variable)
    , attributes(PHP_Access_Public)
    , lineNumber(-1)
    , terminatingOperator(0)
    , prefixOperator(0)
{
}

void PHPEntry::save(wxSQLite3Database* db, long parentDbId)
{
    try {
        wxSQLite3Statement st = db->PrepareStatement("REPLACE INTO ENTRY_TABLE (ID, PARENT_ID, NAME, KIND, TYPE, TYPE_SCOPE, INHERITS, USING_TRAITS, IMPLEMENTS, DOC_COMMENT, ATTRIBUTES, DEFAULT_VALUE, NAMESPACE, LINE_NUMBER, FILE_NAME) VALUES ( NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
        st.Bind(1, (int)parentDbId);
        st.Bind(2, name);
        st.Bind(3, (int)kind);
        st.Bind(4, type);
        st.Bind(5, typeScope);
        st.Bind(6, inherits);
        st.Bind(7, ::wxJoin(getUsingTraits(), ';'));
        st.Bind(8, ::wxJoin(getImplements(), ';'));
        st.Bind(9, docComment);
        st.Bind(10, (int)attributes);
        st.Bind(11, defaultValue);
        st.Bind(12, nameSpace);
        st.Bind(13, lineNumber);
        st.Bind(14, fileName);
        st.ExecuteUpdate();

        dbId = db->GetLastRowId().ToLong();

    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }

}

void PHPEntry::addNewLocal(const wxString& name, const wxString& type, PHPModifier attr)
{
    bool b = type.find_first_not_of(wxT("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_\\->$")) == wxString::npos;
    bool validType = b && type.find_first_of(wxT("0123456789")) != 0; // first char must not start with a number

    PHPEntry local;
    local.setName(name);
    local.setType(validType ? type : wxString());
    local.setKind(PHP_Kind_Variable);
    local.addAttribute(attr);
    local.setLineNumber(PHPParser.getLineNumber());
    local.setFileName(PHPParser.getCurrentFileName());
    local.setNameSpace( PHPParser.getCurrentNamespace() );

    // if the current line number is one line below the
    // collected doc comment, attach the doc comment to this
    // function
    if((PHPParser.getLineNumber() - 1) == PHPParser.getDocCommentLine()) {
        local.setDocComment( PHPParser.getDocComment() );
    } else {
        PHPParser.clearDocComment();
    }

    if(locals.find(name) == locals.end()) {
        locals[name] = local;
    }
}

void PHPEntry::setName(const wxString& name)
{
    this->name = name;
    this->lineNumber = PHPParser.getLineNumber();
}

void PHPEntry::newArgument(const wxString& name)
{
    PHPFuncArg arg;
    arg.setName(name);
    arg.setFilename(PHPParser.getCurrentFileName());
    arg.setIndex(arguments.size());
    arguments.push_back(arg);
}

std::map<wxString,PHPEntry> PHPEntry::getLocals() const
{
    std::map<wxString, PHPEntry> locs;
    if(getKind() == PHP_Kind_Function) {
        for(size_t i=0; i<arguments.size(); i++) {
            PHPEntry entry;
            entry.setName(arguments.at(i).getName());
            entry.setFileName(arguments.at(i).getFilename());
            locs[entry.getName()] = entry;
        }
    }
    locs.insert(locals.begin(), locals.end());
    return locs;
}

void PHPEntry::addArgument(const PHPFuncArg& arg)
{
    this->arguments.push_back(arg);
}

wxString PHPEntry::getType() const
{
    // if we got a docComment, use the @return statement
    if(docComment.IsEmpty() == false) {

        PHPDocComment phpComment(docComment);
        if((getKind() == PHP_Kind_Variable || getKind() == PHP_Kind_Member) && !phpComment.getVar().IsEmpty()) {
            return phpComment.getVar();

        } else if(getKind() == PHP_Kind_Function && !phpComment.getReturn().IsEmpty()) {
            return phpComment.getReturn();
        }

    } else if ( PHPParser.getCurrentEntry().isFunction() && type.IsEmpty() ) {
        // type is empty and we are inside a function body
        // try to match this entry to one of the function arguments
        const PHPEntry& func = PHPParser.getCurrentEntry();
        const PHPFuncArg *arg = func.findArgument(getName());
        if ( arg ) {
            return arg->getType(func.getDocComment());
        }
    }
    return type;
}

void PHPEntry::sortArgs()
{
    std::sort(arguments.begin(), arguments.end(), _SAscendingSortArgs());
}

PHPEntry::Vector_t PHPEntry::loadByParentId(wxSQLite3Database* db, long parentDbId, bool includeChildren)
{
    PHPEntry::Vector_t entries;
    try {
        wxString sql;
        sql << "SELECT * FROM ENTRY_TABLE WHERE " << DoMakeParentIdSql(db, parentDbId) << "ORDER BY KIND DESC";
        DoAddLimitClause( sql );
        wxSQLite3Statement st = db->PrepareStatement( sql);

        wxSQLite3ResultSet res = st.ExecuteQuery();
        while ( res.NextRow() ) {
            PHPEntry entry = PHPEntry::fromResultSet( db , res, includeChildren );
            entries.push_back( entry );
        }

    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
    return entries;
}

PHPEntry::Vector_t PHPEntry::loadByParentId(wxSQLite3Database* db, long parentDbId, size_t flags)
{
    PHPEntry::Vector_t entries;
    try {
        wxString sql;
        if ( (flags & PHPStorage::FetchFlags_Self) || (flags & PHPStorage::FetchFlags_Self_All) ) {
            // only this class, without its parents
            sql << "SELECT * FROM ENTRY_TABLE WHERE PARENT_ID = " << parentDbId;
            
        } else {
            sql << "SELECT * FROM ENTRY_TABLE WHERE " << DoMakeParentIdSql(db, parentDbId);
            
        }
        
        // functions only?
        if ( flags & PHPStorage::FetchFlags_FunctionsOnly ) {
            sql << " AND KIND=" << (int) PHP_Kind_Function;
        }
        
        // static only ?
        bool staticOnly = (flags & PHPStorage::FetchFlags_Self) || (flags & PHPStorage::FetchFlags_Static);
        
        sql << " ORDER BY KIND DESC";
        DoAddLimitClause( sql );
        wxSQLite3Statement st = db->PrepareStatement( sql);
        wxSQLite3ResultSet res = st.ExecuteQuery();
        while ( res.NextRow() ) {
            PHPEntry entry = PHPEntry::fromResultSet( db , res, flags & PHPStorage::FetchFlags_FullClass );
            // If we are collecting only static members and the entry does not have the 
            // static attribute - ignore it.
            // If we are collecting static members and the type of the entry is of 'member'
            // ensure that its name start with $
            if ( staticOnly && !(entry.getAttributes() & PHP_Attr_Static) ) {
                continue;
            }
            entries.push_back( entry );
        }

    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
    return entries;
}

bool PHPEntry::loadById(wxSQLite3Database* db, long dbId, PHPEntry& entry, bool includeChildren)
{
    try {
        wxSQLite3Statement st = db->PrepareStatement("SELECT * FROM ENTRY_TABLE WHERE ID = ? LIMIT 1");
        st.Bind(1, (int)dbId);

        wxSQLite3ResultSet res = st.ExecuteQuery();
        if ( res.NextRow() ) {
            entry = PHPEntry::fromResultSet( db , res, includeChildren );
            return true;
        }

    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
    return false;
}

bool PHPEntry::loadByNameAndNS(wxSQLite3Database* db, const wxString& name, const wxString& ns, PHPEntry& cls, PHPEntryKind kind, bool includeChildren)
{
    try {

        wxString real_name, real_ns;
        CorrectClassName(name, ns, real_name, real_ns);

        wxString sql;
        if ( kind == PHP_Kind_Unknown ) {
            sql << "SELECT * FROM ENTRY_TABLE WHERE NAME = ? AND NAMESPACE = ?";
        } else {
            sql << "SELECT * FROM ENTRY_TABLE WHERE NAME = ? AND NAMESPACE = ? AND KIND = ?";
        }
        DoAddLimitClause( sql );
        wxSQLite3Statement st = db->PrepareStatement( sql );
        st.Bind(1, real_name);
        st.Bind(2, real_ns);

        if ( kind != PHP_Kind_Unknown ) {
            st.Bind(3, (int) PHP_Kind_Class);
        }

        wxSQLite3ResultSet res = st.ExecuteQuery();
        if ( res.NextRow() ) {
            cls = PHPEntry::fromResultSet( db, res, includeChildren );
            return true;
        }

    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
    return false;
}

void PHPEntry::setEntries(const std::vector<PHPEntry>& entries)
{
    this->entries.clear();
    this->entriesMap.clear();

    this->entries = entries;
    for( size_t i=0; i<this->entries.size(); ++i ) {
        entriesMap.insert( std::make_pair(entries.at(i).getName(), i) );
    }
}

wxString PHPEntry::makeFullPath(const wxString& curns) const
{
    if ( getNameSpace().IsEmpty() ) {
        // if the current namespace is empty, the only name space to search
        // the curns
        return wxString() << curns << "\\" << getName();

    } else if ( getNameSpace().StartsWith("\\") ) {
        // an abs path
        return wxString() << getNameSpace() << "\\" << getName();

    } else if ( getNameSpace().Contains("\\") ) {
        // relative namespace
        return wxString() << curns << "\\" << getNameSpace() << "\\" << getName();

    } else {
        return wxString() << curns << "\\" << getName();
    }
}

bool PHPEntry::loadByParentIdAndName(wxSQLite3Database* db, long parentDbId, const wxString& name, PHPEntryKind kind, PHPEntry& entry, bool includeChildren)
{
    try {
        PHPEntry::LongVector_t ids;
        PHPEntry::StringSet_t  parents;

        // parentId may not be enough since the entry might exists in one of the parents of this parent...
        wxString sql;
        sql << "select * from ENTRY_TABLE where PARENT_ID IN(";
        GetParents(db, parentDbId, ids, parents);
        for( size_t i=0; i<ids.size(); ++i ) {
            sql << ids.at(i) << ",";
        }

        if ( sql.EndsWith(",") ) sql.RemoveLast();
        sql << ") AND NAME = ? ";

        if ( kind != PHP_Kind_Unknown ) {
            sql << " AND KIND=? ";
        }
        sql << "LIMIT 1";

        wxSQLite3Statement st = db->PrepareStatement( sql );
        st.Bind(1, name);
        if ( kind != PHP_Kind_Unknown ) {
            st.Bind( 2, (int) kind );
        }

        wxSQLite3ResultSet res = st.ExecuteQuery();
        if ( res.NextRow() ) {
            entry = fromResultSet(db, res, includeChildren);
            return true;
        }

    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
    return false;
}

void PHPEntry::CorrectClassName(const wxString& in_name, const wxString& in_ns, wxString& out_name, wxString& out_ns)
{
    if ( in_name.StartsWith("\\") ) {
        // a fullpath name - ignore the in_ns part
        wxArrayString parts1 = ::wxStringTokenize(in_name, "\\", wxTOKEN_STRTOK);
        out_name = in_name.AfterLast('\\');
        out_ns   = in_name.BeforeLast('\\');

    } else {
        wxArrayString parts1 = ::wxStringTokenize(in_name, "\\", wxTOKEN_STRTOK);
        wxArrayString parts2 = ::wxStringTokenize(in_ns,   "\\", wxTOKEN_STRTOK);
        parts2.insert(parts2.end(), parts1.begin(), parts1.end());
        if ( parts2.IsEmpty() )
            return;

        out_name = parts2.Last();
        parts2.RemoveAt(parts2.GetCount()-1); // Remove last

        out_ns = ::wxJoin(parts2, '\\', '^');
        if ( !out_ns.IsEmpty() && !out_ns.StartsWith("\\") ) {
            out_ns.Prepend("\\");
        }
    }
}

PHPEntry PHPEntry::fromResultSet(wxSQLite3Database* db, wxSQLite3ResultSet& res, bool includeChildren)
{
    PHPEntry entry;
    entry.dbId         = res.GetInt(0);
    entry.parentDbId   = res.GetInt(1);
    entry.name         = res.GetString(2);
    entry.kind         = (PHPEntryKind)res.GetInt(3);
    entry.type         = res.GetString(4);
    entry.typeScope    = res.GetString(5);
    entry.inherits     = res.GetString(6);
    entry.usingTraits  = ::wxSplit(res.GetString(7), ';');
    entry.implements   = ::wxSplit(res.GetString(8), ';');
    entry.docComment   = res.GetString(9);
    entry.attributes   = res.GetInt(10);
    entry.defaultValue = res.GetString(11);
    entry.nameSpace    = res.GetString(12);
    entry.lineNumber   = res.GetInt(13);
    entry.fileName     = res.GetString(14);
    
    entry.FixDollarPrefix(); // Fix name to include $ prefix if needed
    
    if ( entry.isFunction() ) {
        // load function arguments
        entry.setArguments( PHPFuncArg::loadByEntryId(db, entry.getDbId()) );
    }

    if ( includeChildren ) {
        // pass 'includeChildren' as false here to avoid recursion
        PHPEntry::LongVector_t ids;
        PHPEntry::StringSet_t  parents;
        GetParents(db, entry, ids, parents);

        PHPEntry::Vector_t entries;
        for(size_t i=0; i<ids.size(); ++i) {
            PHPEntry::Vector_t tmpRes = PHPEntry::loadByParentId(db, ids.at(i), false);
            entries.insert(entries.end(), tmpRes.begin(), tmpRes.end());
        }
        entry.setEntries( entries );
    }
    return entry;
}

wxString PHPEntry::getTypeInfoTooltip() const
{
    wxString tooltip;
    switch ( getKind() ) {
    case PHP_Kind_Class:
        tooltip << "<b>Class: </b>" << getName() << "\n";
        tooltip << "<b>Namespace: </b>" << (getNameSpace().IsEmpty() ? wxString("Global") : getNameSpace());
        break;
    case PHP_Kind_Function: {
        tooltip << "<b>Function: </b>" << getName() << "\n";
        tooltip << "<b>Signature: </b>" << getSignature();

        if ( !getDocComment().IsEmpty() ) {
            wxString docComment = getDocComment();
            docComment.Trim().Trim(false);
            tooltip << "\n<hr>" << docComment;
        }
    }
    break;
    case PHP_Kind_Namespace: {
        tooltip << "<b>Namespace: </b>" << getName();

    }
    break;
    case PHP_Kind_Variable: {

        tooltip << "<b>Variable: </b>" << getName() << "\n";
        if ( !getType().IsEmpty() ) {
            tooltip << "<b>Type: </b>" << getType();
        }
    }
    break;
    default:
        break;
    }
    return tooltip;
}

wxString PHPEntry::getSignature() const
{
    wxString sig;
    sig << "(";
    for(size_t i=0; i<arguments.size(); ++i) {
        wxString str;
        PHPFuncArg arg = arguments.at(i);
        if ( !arg.getType().IsEmpty() ) {
            str << arg.getType() << " ";
        }
        str << arg.getName() << ", ";
        sig << str;
    }

    if ( sig.EndsWith(", ") ) {
        sig.RemoveLast(2);
    }
    sig << ")";
    return sig;
}

void PHPEntry::GetParents(wxSQLite3Database* db, const wxString& name, const wxString& nameSpace, PHPEntry::LongVector_t& ids, PHPEntry::StringSet_t &scannedParents)
{
    wxString classname, name_space;
    CorrectClassName(name, nameSpace, classname, name_space);

    PHPEntry cls;
    if ( !PHPEntry::loadByNameAndNS(db, classname, name_space, cls) )
        return;

    GetParents(db, cls, ids, scannedParents);
}

void PHPEntry::GetParents(wxSQLite3Database* db, const PHPEntry& cls, PHPEntry::LongVector_t& ids, PHPEntry::StringSet_t& scannedParents)
{
    ids.push_back( cls.getDbId() );
    scannedParents.insert( cls.getName() );

    wxArrayString inhertis;
    for(size_t i=0; i<cls.getImplements().GetCount(); i++) {
        if( !scannedParents.count(cls.getImplements().Item(i)) ) {
            inhertis.Add(cls.getImplements().Item(i));
            scannedParents.insert(cls.getImplements().Item(i));
        }
    }

    for(size_t i=0; i<cls.getUsingTraits().GetCount(); i++) {
        if( !scannedParents.count(cls.getUsingTraits().Item(i)) ) {
            inhertis.Add(cls.getUsingTraits().Item(i));
            scannedParents.insert(cls.getUsingTraits().Item(i));
        }
    }

    if( !cls.getInherits().IsEmpty() && !scannedParents.count(cls.getInherits()) ) {
        inhertis.Add(cls.getInherits());
        scannedParents.insert(cls.getInherits());
    }

    for(size_t i=0; i<inhertis.GetCount(); i++) {
        GetParents(db, inhertis.Item(i), cls.getNameSpace(), ids, scannedParents);
    }
}

void PHPEntry::GetParents(wxSQLite3Database* db, long dbId, PHPEntry::LongVector_t& ids, PHPEntry::StringSet_t& scannedParents)
{
    if ( dbId == wxNOT_FOUND ) {
        ids.push_back( dbId );
        return;
    }

    PHPEntry cls;
    if ( !PHPEntry::loadById(db, dbId, cls) )
        return;
    GetParents(db, cls, ids, scannedParents);
}

PHPEntry::Vector_t PHPEntry::loadByParentIdAndName(wxSQLite3Database* db, long parentDbId, const wxString& name, bool partialMatch, bool includeChildren)
{
    PHPEntry::Vector_t v;
    try {
        wxString sql;
        wxSQLite3Statement st;
        if ( partialMatch ) {

            wxString escapedName = EscapeString( name );
            if ( !escapedName.IsEmpty() ) {
                sql << "select * from ENTRY_TABLE where " << DoMakeParentIdSql(db, parentDbId) << " AND NAME like '" << escapedName << "%%' ESCAPE '^' ORDER BY KIND DESC";
                DoAddLimitClause( sql );
                st = db->PrepareStatement(sql);

            } else {
                sql << "select * from ENTRY_TABLE where " << DoMakeParentIdSql(db, parentDbId) << "ORDER BY KIND DESC";
                DoAddLimitClause( sql );
                st = db->PrepareStatement(sql);
            }

        } else {
            sql << "select * from ENTRY_TABLE where " << DoMakeParentIdSql(db, parentDbId) <<  " AND NAME = ? LIMIT 1";
            st = db->PrepareStatement(sql);
            st.Bind(1, name);

        }
        wxSQLite3ResultSet res = st.ExecuteQuery();
        while ( res.NextRow() ) {
            PHPEntry match = fromResultSet(db, res, includeChildren);
            v.push_back( match );
        }
    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
    return v;
}

PHPEntry::Vector_t PHPEntry::loadByName(wxSQLite3Database* db, const wxString& filter, bool partialMatch, bool includeChildren)
{
    PHPEntry::Vector_t v;
    try {
        wxString sql;
        wxSQLite3Statement st;
        if ( partialMatch ) {

            wxString escapedName = EscapeString( filter );

            sql << "select * from ENTRY_TABLE where NAME like '" << escapedName << "%%' ESCAPE '^' ORDER BY KIND DESC";
            DoAddLimitClause( sql );
            st = db->PrepareStatement(sql);

        } else {
            sql << "select * from ENTRY_TABLE where NAME = ? ORDER BY KIND DESC";
            DoAddLimitClause( sql );
            st = db->PrepareStatement(sql);
            st.Bind(1, filter);

        }
        wxSQLite3ResultSet res = st.ExecuteQuery();
        while ( res.NextRow() ) {
            PHPEntry match = fromResultSet(db, res, includeChildren);
            v.push_back( match );
        }
    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
    return v;
}

wxString PHPEntry::DoMakeParentIdSql(wxSQLite3Database* db, long parentId)
{
    PHPEntry::LongVector_t ids;
    PHPEntry::StringSet_t  parents;

    // parentId may not be enough since the entry might exists in one of the parents of this parent...
    wxString sql;
    sql << " PARENT_ID IN(";
    GetParents(db, parentId, ids, parents);

    for( size_t i=0; i<ids.size(); ++i ) {
        sql << ids.at(i) << ",";
    }

    if ( sql.EndsWith(",") ) sql.RemoveLast();
    sql << ") ";
    return sql;
}

wxString PHPEntry::FormatDoxygenComment() const
{
    // Honor the editor settings
    CommentConfigData data;
    EditorConfigST::Get()->ReadObject(wxT("CommentConfigData"), &data);

    //get doxygen comment based on file and line
    wxChar keyPrefix(wxT('\\'));
    if (data.GetUseShtroodel()) {
        keyPrefix = wxT('@');
    }

    wxString starter;
    data.GetUseSlash2Stars() ? starter = "/**\n" : starter = "/*!\n";
    wxString comment;
    if ( isFunction() ) {

        comment << starter;
        std::vector<PHPFuncArg> func_args = getArguments();
        for(size_t i=0; i<func_args.size(); ++i) {
            // format a string of the form:
            // @param <name> <type>
            const PHPFuncArg& arg = func_args.at(i);
            wxString arg_name = arg.getName();
            wxString arg_type = arg.getType();
            comment << " * " << keyPrefix << "param " << arg_name << " " << arg_type << "\n";
        }

        // If the function has a return value, add it
        comment << " * " << keyPrefix << "return " << getType() << "\n";
        comment << " */\n";
    }
    return comment;
}

PHPEntry::Vector_t PHPEntry::FindSuperGloabls(wxSQLite3Database* db, const wxString& name)
{
    PHPEntry::Vector_t arr;
    try {
        wxString sql;
        wxString escapedName = EscapeString( name );
        sql << "select * from ENTRY_TABLE where NAME like '" << escapedName << "%%' ESCAPE '^' AND FILE_NAME LIKE '%%SuperGlobals.php'";
        DoAddLimitClause( sql );
        wxSQLite3Statement st = db->PrepareStatement( sql );
        wxSQLite3ResultSet res = st.ExecuteQuery();
        while ( res.NextRow() ) {
            arr.push_back( PHPEntry::fromResultSet( db, res, false ) );
        }

    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
    return arr;
}

PHPEntry::Vector_t PHPEntry::FindGloablFunctions(wxSQLite3Database* db, const wxString& name)
{
    PHPEntry::Vector_t arr;
    try {
        wxString sql;
        wxString escapedName = EscapeString( name );
        sql << "select * from ENTRY_TABLE where NAME like '" << escapedName << "%%' ESCAPE '^' AND KIND IN(" << (int) PHP_Kind_Function << ") ";
        DoAddLimitClause( sql );
        wxSQLite3Statement st = db->PrepareStatement( sql );
        wxSQLite3ResultSet res = st.ExecuteQuery();
        while ( res.NextRow() ) {
            arr.push_back( PHPEntry::fromResultSet( db, res, false ) );
        }

    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
    return arr;
}

PHPEntry::Vector_t PHPEntry::LoadNamespaceEntries(wxSQLite3Database* db, const wxString& ns, const wxString& partname, size_t flags)
{
    PHPEntry::Vector_t arr;
    try {
        wxString sql;
        wxString fixedNamespace = ns;
        fixedNamespace.Trim().Trim(false);
        if ( fixedNamespace == "\\" ) {
            fixedNamespace.Clear();

        } else if ( !fixedNamespace.StartsWith("\\") && !fixedNamespace.IsEmpty() ) {
            fixedNamespace.Prepend("\\");
        }

        sql << "select * from ENTRY_TABLE where NAMESPACE = ? AND KIND IN (" << (int)PHP_Kind_Class << ", " << (int)PHP_Kind_Namespace << ") ";

        if ( !partname.IsEmpty() ) {
            wxString escapedName = EscapeString( partname );
            sql << " AND NAME like '" << escapedName << "%%' ESCAPE '^' ";
        }
        DoAddLimitClause( sql );

        wxSQLite3Statement st = db->PrepareStatement( sql );
        st.Bind(1, fixedNamespace);

        wxSQLite3ResultSet res = st.ExecuteQuery();
        while ( res.NextRow() ) {
            arr.push_back( PHPEntry::fromResultSet( db, res, flags & PHPStorage::FetchFlags_FullClass ) );
        }

    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
    return arr;
}

wxString PHPEntry::getContainerPath() const
{
    wxString fullpath;
    fullpath << getNameSpace() << "\\" << getName();
    fullpath.Replace("\\\\", "\\");
    if ( !fullpath.StartsWith("\\") ) {
        fullpath.Prepend("\\");
    }
    return fullpath;
}

void PHPEntry::DoAddLimitClause(wxString& sql)
{
    sql << " LIMIT " << PHPStorage::Instance()->GetLimit();
}

void PHPEntry::addInterface(const wxString& iface)
{
    implements.Add(iface);
}

void PHPEntry::setInherits(const wxString& inherits)
{
    this->inherits = inherits;
}

void PHPEntry::FixDollarPrefix()
{
    if ( isStatic() && isMember() ) {
        if(!name.StartsWith("$")) {
            name.Prepend("$");
        }
        
    } else if ( isMember() ) {
        if ( name.StartsWith("$") ) {
            name.Remove(0, 1);
        }
    }
}
