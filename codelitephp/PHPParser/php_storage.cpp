#include "php_storage.h"
#include "php_workspace.h"
#include <plugin.h>
#include <globals.h>
#include "php_project_settings_data.h"
#include <dirtraverser.h>
#include <event_notifier.h>
#include <wx/sharedptr.h>
#include "php_code_completion.h"
#include "php_parser_thread.h"
#include "codelite_events.h"

#ifndef __WXMSW__
#include <wx/ffile.h>
#include <wx/arrstr.h>
#include <wx/tokenzr.h>
#endif

//===========================================================================================
// ENTRY_TABLE
//===========================================================================================
const static wxString CREATE_ENTRY_TABLE_SQL = 
"CREATE TABLE IF NOT EXISTS ENTRY_TABLE( ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
"PARENT_ID INTEGER NOT NULL DEFAULT -1, "
"NAME TEXT, "
"KIND INTEGER , "
"TYPE TEXT, "
"TYPE_SCOPE TEXT, "
"INHERITS TEXT, "
"USING_TRAITS TEXT, "
"IMPLEMENTS TEXT, "
"DOC_COMMENT TEXT, "
"ATTRIBUTES INTEGER NOT NULL DEFAULT 0, "
"DEFAULT_VALUE TEXT NOT NULL DEFAULT \"\", "
"NAMESPACE TEXT NOT NULL DEFAULT \"\", "
"LINE_NUMBER INTEGER  NOT NULL DEFAULT 0, "
"FILE_NAME TEXT )";

const static wxString CREATE_ENTRY_TABLE_SQL_IDX1 = 
"CREATE INDEX IF NOT EXISTS ENTRY_TABLE_IDX_1        ON ENTRY_TABLE(NAME)";
const static wxString CREATE_ENTRY_TABLE_SQL_IDX2 = 
"CREATE UNIQUE INDEX IF NOT EXISTS ENTRY_TABLE_IDX_2 ON ENTRY_TABLE(NAME, PARENT_ID, NAMESPACE)";
const static wxString CREATE_ENTRY_TABLE_SQL_IDX3 = 
"CREATE INDEX IF NOT EXISTS ENTRY_TABLE_IDX_3        ON ENTRY_TABLE(PARENT_ID)";
const static wxString CREATE_ENTRY_TABLE_SQL_IDX4 = 
"CREATE INDEX IF NOT EXISTS ENTRY_TABLE_IDX_4        ON ENTRY_TABLE(FILE_NAME)";
const static wxString CREATE_ENTRY_TABLE_SQL_IDX5 = 
"CREATE INDEX IF NOT EXISTS ENTRY_TABLE_IDX_5        ON ENTRY_TABLE(LINE_NUMBER)";

//===========================================================================================
// FUNC_ARGS
//===========================================================================================
const static wxString CREATE_FUNC_ARGS_SQL = 
"CREATE TABLE IF NOT EXISTS FUNC_ARGS( ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
"ENTRY_ID INTEGER NOT NULL, "
"NAME TEXT, "
"TYPE TEXT, "
"LINE_NUMBER INTEGER NOT NULL DEFAULT 0, "
"ATTRIBUTES INTEGER NOT NULL DEFAULT 0, "
"FILE_NAME TEXT, "
"ARG_INDEX INTEGER NOT NULL DEFAULT 0)";

const static wxString CREATE_FUNC_ARGS_SQL_IDX1 = 
"CREATE INDEX IF NOT EXISTS FUNC_ARGS_IDX_1 ON FUNC_ARGS(ENTRY_ID)";

//===========================================================================================
// FILES_TABLE
//===========================================================================================
const static wxString CREATE_FILES_TABLE_SQL = 
"CREATE TABLE IF NOT EXISTS FILES_TABLE( ID INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, "
"FILE_NAME TEXT, "
"LAST_UPDATED INTEGER)";

const static wxString CREATE_FILES_TABLE_SQL_IDX1 = 
"CREATE UNIQUE INDEX IF NOT EXISTS FILES_TABLE_IDX_1 ON FILES_TABLE(FILE_NAME)";

//===========================================================================================
//===========================================================================================


#define CHECK_IS_OPEN_RETURN() if(!IsOpen()) return;
#define CHECK_IS_OPEN_RETURN_FALSE() if(!IsOpen()) return false;

#include <progress_dialog.h>
#define StripNamespace(s) s = s.AfterLast(wxT('\\'))
#define _cstr(str) str.mb_str(wxConvUTF8).data()

PHPStorage* PHPStorage::ms_instance = 0;

PHPStorage::PHPStorage()
    : m_db(NULL)
    , m_limit(150)
{
}

PHPStorage::~PHPStorage()
{
    EventNotifier::Get()->Disconnect(wxEVT_PROJ_FILE_ADDED,          clCommandEventHandler(PHPStorage::OnFileAdded),          NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PROJ_FILE_REMOVED,        clCommandEventHandler(PHPStorage::OnFilesRemoved),       NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_FILE_SAVED,               clCommandEventHandler(PHPStorage::OnFileSaved),          NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_RETAG_WORKSPACE,      wxCommandEventHandler(PHPStorage::OnRetagWorkspace),     NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_CMD_RETAG_WORKSPACE_FULL, wxCommandEventHandler(PHPStorage::OnRetagWorkspaceFull), NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_WORKSPACE_CLOSED,         wxCommandEventHandler(PHPStorage::OnWorkspaceClosed),    NULL, this);
    EventNotifier::Get()->Disconnect(wxEVT_PHP_WORKSPACE_RENAMED,    PHPEventHandler(PHPStorage::OnWorkspaceRenamed),    NULL, this);
}

void PHPStorage::ConnectEvents()
{
    EventNotifier::Get()->Connect(wxEVT_PROJ_FILE_ADDED,          clCommandEventHandler(PHPStorage::OnFileAdded),          NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PROJ_FILE_REMOVED,        clCommandEventHandler(PHPStorage::OnFilesRemoved),       NULL, this);
    EventNotifier::Get()->Connect(wxEVT_FILE_SAVED,               clCommandEventHandler(PHPStorage::OnFileSaved),          NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_RETAG_WORKSPACE,      wxCommandEventHandler(PHPStorage::OnRetagWorkspace),     NULL, this);
    EventNotifier::Get()->Connect(wxEVT_CMD_RETAG_WORKSPACE_FULL, wxCommandEventHandler(PHPStorage::OnRetagWorkspaceFull), NULL, this);
    EventNotifier::Get()->Connect(wxEVT_WORKSPACE_CLOSED,         wxCommandEventHandler(PHPStorage::OnWorkspaceClosed),    NULL, this);
    EventNotifier::Get()->Connect(wxEVT_PHP_WORKSPACE_RENAMED,    PHPEventHandler(PHPStorage::OnWorkspaceRenamed),    NULL, this);
}

PHPStorage* PHPStorage::Instance()
{
    if(ms_instance == 0) {
        ms_instance = new PHPStorage();
    }
    return ms_instance;
}

void PHPStorage::Release()
{
    if(ms_instance) {
        delete ms_instance;
    }
    ms_instance = 0;
}

void PHPStorage::OpenWorksace(const wxFileName &dbfile, bool retagOnOpen, bool fullRetag)
{
    Close();
    
    wxFileName fnDBFile = dbfile;
    fnDBFile.SetExt("phptags");
    try {
        m_db = new wxSQLite3Database();
        m_db->Open(fnDBFile.GetFullPath());
        CreateSchema();
        
    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
        return;
    }
        
    if(!IsOpen())
        return;

    m_dbfile = fnDBFile.GetFullPath();
    if(retagOnOpen) {
        ReparseFiles(fullRetag);
    }
}

void PHPStorage::Begin()
{
    try {
        m_db->Begin();
    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
}

void PHPStorage::Commit()
{
    try {
        m_db->Commit();
    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
}


void PHPStorage::DeleteByFile(const wxFileName& filename)
{
    try {
        
        // Delete all entries owned by this file name
        wxSQLite3Statement st2 = m_db->PrepareStatement("delete from ENTRY_TABLE where FILE_NAME=?");
        wxSQLite3Statement st3 = m_db->PrepareStatement("delete from FUNC_ARGS where FILE_NAME=?");
        wxSQLite3Statement st4 = m_db->PrepareStatement("delete from FILES_TABLE where FILE_NAME=?");
        
        st2.Bind(1, filename.GetFullPath());
        st2.ExecuteUpdate();
        
        st3.Bind(1, filename.GetFullPath());
        st3.ExecuteUpdate();
        
        st4.Bind(1, filename.GetFullPath());
        st4.ExecuteUpdate();
        
    } catch (wxSQLite3Exception &e) {
        wxUnusedVar( e );
    }
}

void PHPStorage::Close()
{
    if( IsOpen() ) {
        m_db->Close();
    }
    m_dbfile.Clear();
    wxDELETE(m_db);
}

void PHPStorage::ParseAndStoreFileNoLock(const wxString& filename, bool commit, bool deleteBeforeInsert)
{
    wxASSERT_MSG( !wxThread::IsMain(), "ParseFileNoLock can be called only from the parser thread!");
    
    PHPParser.parseFile(false, filename, PHPGlobals::Type_Normal, true);
    PHPStorage::AutoCommitter ac(this, commit);
    
    if( deleteBeforeInsert ) {
        DeleteByFile( wxFileName(filename) );
    }
    
    // Save all namespaces
    PHPEntry::List_t &nsList = PHPParser.getNamespaceVisitedList();
    PHPEntry::List_t::iterator itNsList = nsList.begin();
    for(; itNsList != nsList.end(); ++itNsList) {
        DoStore(*itNsList, wxNOT_FOUND);
    }
    
    // save all classes
    PHPEntry::VectorPtr_t& classes = PHPParser.getClasses();
    for(size_t i=0; i<classes.size(); ++i) {
        DoStore(*classes.at(i), wxNOT_FOUND);
    }
    
    std::map<wxString, PHPEntry> globals = PHPParser.getGlobals();
    std::map<wxString, PHPEntry>::iterator iter = globals.begin();
    for( ; iter != globals.end(); ++iter ) {
        DoStore(iter->second, wxNOT_FOUND); 
    }
    
    DoUpdateFile( filename );
}

void PHPStorage::ParseFile(const wxString& filename, bool commit, bool deleteBeforeInsert /* true */)
{
    if ( !PHPParser.parseFile(true, filename, PHPGlobals::Type_Normal, true) ) {
        return;
    }
    
    PHPStorage::AutoCommitter ac(this, commit);
    
    if( deleteBeforeInsert ) {
        DeleteByFile( wxFileName(filename) );
    }
    
    // Save all namespaces
    PHPEntry::List_t &nsList = PHPParser.getNamespaceVisitedList();
    PHPEntry::List_t::iterator itNsList = nsList.begin();
    for(; itNsList != nsList.end(); ++itNsList) {
        DoStore(*itNsList, wxNOT_FOUND);
    }
    
    // save all classes
    PHPEntry::VectorPtr_t& classes = PHPParser.getClasses();
    for(size_t i=0; i<classes.size(); ++i) {
        DoStore(*classes.at(i), wxNOT_FOUND);
    }
    
    std::map<wxString, PHPEntry> globals = PHPParser.getGlobals();
    std::map<wxString, PHPEntry>::iterator iter = globals.begin();
    for( ; iter != globals.end(); ++iter ) {
        DoStore(iter->second, wxNOT_FOUND); 
    }
    
    DoUpdateFile( filename );
}

bool PHPStorage::IsOpen() const
{
    return m_db && m_db->IsOpen();
}

void PHPStorage::OnFileAdded(clCommandEvent& event)
{
    event.Skip();
    CHECK_IS_OPEN_RETURN();
    
    if ( event.GetInt() & kEventImportingFolder ) {
        ReparseFiles( false );
        
    } else {
        const wxArrayString& files = event.GetStrings();
        if( !files.IsEmpty() ) {
            DoReparseFiles( files, true, false );
        }
    }
}

void PHPStorage::DoGetParents(const wxString& name, const wxString& nameSpace, std::vector<long> &ids, std::set<wxString> &scannedParents)
{
    CHECK_IS_OPEN_RETURN();
    
    wxString classname, name_space;
    DoCorrectClassName(name, nameSpace, classname, name_space);
    
    PHPEntry cls;
    if ( !PHPEntry::loadByNameAndNS(m_db, classname, name_space, cls) )
        return;

    DoGetParents(cls, ids, scannedParents);
}

void PHPStorage::DoGetParents(const PHPEntry& cls, std::vector<long> &ids, std::set<wxString> &scannedParents)
{
    CHECK_IS_OPEN_RETURN();
    
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
        DoGetParents(inhertis.Item(i), cls.getNameSpace(), ids, scannedParents);
    }
}

void PHPStorage::DoCorrectClassName(const wxString& in_name, const wxString& in_ns, wxString& out_name, wxString& out_ns)
{
    PHPEntry::CorrectClassName(in_name, in_ns, out_name, out_ns);
}

void PHPStorage::FetchClasses(const wxString& name, const wxString& nameSpace, std::vector<PHPEntry>& classes, size_t searchFlags)
{
    if(!IsOpen()) {
        return;
    }
    
    if ( searchFlags & PHPStorage::FetchFlags_ExactMatch ) {
        // A single class 
    
        // Fix the class name and namespace
        wxString out_class, out_namespace;
        wxArrayString namespaceToTry;
        if ( name.StartsWith("\\") ) {
            
            // a full path
            DoCorrectClassName(name, "", out_class, out_namespace);
            namespaceToTry.Add( out_namespace );
            
        } else if ( name.Contains("\\") ) {
            
            // Partial namespace?
            DoCorrectClassName(name, nameSpace, out_class, out_namespace);
            namespaceToTry.Add( out_namespace );
            
            out_namespace.Clear();
            out_class.Clear();
            
            DoCorrectClassName(name, "", out_class, out_namespace);
            namespaceToTry.Add( out_namespace );
            
        } else {
            // No namespace - use either global or use the current one
            DoCorrectClassName(name, nameSpace, out_class, out_namespace);
            namespaceToTry.Add(out_namespace);  // current

        }
        
        for(size_t i=0; i<namespaceToTry.GetCount(); ++i) {
            // get list of IDs from the database
            wxStringSet_t scannedParents;
            std::vector<long> ids;
            DoGetParents(out_class, namespaceToTry.Item(i), ids, scannedParents);
            
            // ids.at(0) contains the id of the requested class
            // the rest are the traits/inherits/implements class IDs
            if ( ids.size() ) {
                PHPEntry entry;
                if ( PHPEntry::loadById(m_db, ids.at(0), entry, searchFlags & PHPStorage::FetchFlags_FullClass ) ) {
                    classes.push_back( entry );
                    break;
                }
            }
        }
    }
}

PHPEntry::Vector_t PHPStorage::FetchNamespaceEntries(const wxString& nameSpace, const wxString& partName, size_t flags)
{
    if(!IsOpen()) {
        return PHPEntry::Vector_t();
    }
    return PHPEntry::LoadNamespaceEntries(m_db, nameSpace, partName, flags);
}

bool PHPStorage::FindClass(const wxString& name, const wxString& nameSpace, size_t searchFlags, PHPEntry& cls)
{
    CHECK_IS_OPEN_RETURN_FALSE();
    std::vector<PHPEntry> res;
    FetchClasses(name, nameSpace, res, searchFlags);
    if(res.size() == 1) {
        cls = res.at(0);
    }
    return res.size() == 1;
}

void PHPStorage::DoReparseFiles(const wxArrayString& files, bool full_retag, bool deleteBeforeInsert)
{
    CHECK_IS_OPEN_RETURN();
    
    bool inUnitTestsMode = PHPCodeCompletion::Instance()->IsUnitTestsMode();
    wxSharedPtr<clProgressDlg> prgDlg;
    
    if(!files.IsEmpty() && IsOpen()) {
        Begin();
        
        if ( !inUnitTestsMode ) {
            prgDlg = new clProgressDlg(EventNotifier::Get()->TopFrame(), _("Parsing files..."), wxT(""), files.GetCount());
        }
        
        for(size_t i=0; i<files.GetCount(); i++) {
            ParseFile(files.Item(i), false, deleteBeforeInsert);
            if(i % 1000 == 0) {
                Commit();
                Begin();
            }

            wxFileName fn(files.Item(i));
            fn.MakeRelativeTo(PHPWorkspace::Get()->GetFilename().GetPath());
            if ( prgDlg ) {
                prgDlg->Update(i,wxString() << "Parsing file " << i << "/" << files.GetCount() << ": " << fn.GetFullName() );
                
            } else {
                wxPrintf("%s\n", wxString() << "Parsing file " << i << "/" << files.GetCount() << ": " << fn.GetFullName());
            }
        }
        Commit();
    }
}

void PHPStorage::ReparseFiles(bool full_retag)
{
    CHECK_IS_OPEN_RETURN();
    wxArrayString workspaceFilesArray, filteredFiles;
    wxStringSet_t workspaceFiles;
    PHPWorkspace::Get()->GetWorkspaceFiles( workspaceFiles );

    // get the list of extra files from the additional include paths
    // and merge them with the workspace file
    wxArrayString extraFiles;
    DoGetExternalFiles(extraFiles);
    workspaceFiles.insert(extraFiles.begin(), extraFiles.end());

    // Prepare an array list from the set
    workspaceFilesArray.clear();
    std::set<wxString>::iterator iter = workspaceFiles.begin();
    for(; iter != workspaceFiles.end(); iter++) {
        workspaceFilesArray.Add((*iter));
    }

    if(full_retag) {
        // Remove the database and then perform a retagging
        wxString file_name = m_dbfile; // Close() will clear m_dbfile...
        Close();
        {
            wxLogNull nl;
            ::wxRemoveFile( file_name );
        }
        
        OpenWorksace(file_name, false);
        //DoReparseFiles(workspaceFilesArray, full_retag, false);
        QueueParsingRequest(workspaceFilesArray, full_retag);

    } else {
        // filter the files that don't really need parsing
        DoFilterFilesThatDontNeedReparsing(workspaceFilesArray, filteredFiles);
        //DoReparseFiles(filteredFiles, full_retag, true);
        QueueParsingRequest(filteredFiles, false);
    }
}

bool PHPStorage::GetAllResources(ResourceVector_t& classes, const wxString &filter)
{
    CHECK_IS_OPEN_RETURN_FALSE();
    PHPEntry::Vector_t res = PHPEntry::loadByName(m_db, filter, PHPStorage::FetchFlags_PartMatch);
    for(size_t i=0; i<res.size(); ++i) {
        classes.push_back( ResourceItem( res.at(i) ) );
    }
    return !classes.empty();
}

void PHPStorage::OnFilesRemoved(clCommandEvent& event)
{
    event.Skip();
    if(PHPWorkspace::Get()->IsOpen()) {
        // Remove all symbols belongs to these files
        const wxArrayString &files = event.GetStrings();
        if( !files.IsEmpty() ) {
            Begin();
            for(size_t i=0; i<files.GetCount(); i++) {
                DeleteByFile(files.Item(i));
            }
            Commit();
        }
    }
}


void PHPStorage::OnRetagWorkspace(wxCommandEvent& e)
{
    if(IsOpen()) {
        ReparseFiles(false);

    } else {
        e.Skip();
    }
}

void PHPStorage::OnRetagWorkspaceFull(wxCommandEvent& e)
{
    if(IsOpen()) {
        ReparseFiles(true);

    } else {
        e.Skip();
    }
}

void PHPStorage::OnFileSaved(clCommandEvent& e)
{
    e.Skip();

    CHECK_IS_OPEN_RETURN();
    ParseFile(e.GetString(), true);
}

void PHPStorage::FindSymbol(const wxString& name, std::vector<ResourceItem>& items)
{
    CHECK_IS_OPEN_RETURN();
}

void PHPStorage::DoFilterFilesThatDontNeedReparsing(const wxArrayString& files, wxArrayString &filesNeedsReparsing)
{
    filesNeedsReparsing.Clear();
    for(size_t i=0; i<files.GetCount(); i++) {
        time_t storedTimeStamp = DoGetFileLastUpdatedTimestamp(files.Item(i));
        time_t actualTimeStamp = GetFileModificationTime(files.Item(i));
        if(actualTimeStamp > storedTimeStamp) {
            // This file needs retagging
            filesNeedsReparsing.Add(files.Item(i));
        }
    }
}

time_t PHPStorage::DoGetFileLastUpdatedTimestamp(const wxString &filename)
{
    try {
        wxSQLite3Statement st = m_db->PrepareStatement("select LAST_UPDATED from FILES_TABLE where FILE_NAME=?");
        st.Bind(1, filename);
        wxSQLite3ResultSet res = st.ExecuteQuery();
        if ( res.NextRow() ) {
            return res.GetInt(0);
        }
        
    } catch ( wxSQLite3Exception & e) {
        wxUnusedVar(e);
    }
    return 0;
}

void PHPStorage::DoGetExternalFiles(wxArrayString& files)
{
    // Once the workspace has been loaded, scan the extra include paths
    // and read a list of files that should be parsed
    wxArrayString extraDirs = PHPProjectSettingsData::GetAllIncludePaths();
    DirTraverser traverser(wxT("*.php;*.inc"));

    for(size_t i=0; i<extraDirs.GetCount(); i++) {
        wxDir dir(extraDirs.Item(i));
        dir.Traverse(traverser);
        // append the results
        files.insert(files.end(), traverser.GetFiles().begin(), traverser.GetFiles().end());
    }
}

void PHPStorage::CreateSchema()
{
    try {
        m_db->ExecuteUpdate("pragma synchronous = off");
        
        // Entry Table
        m_db->ExecuteUpdate(CREATE_ENTRY_TABLE_SQL);
        m_db->ExecuteUpdate(CREATE_ENTRY_TABLE_SQL_IDX1);
        m_db->ExecuteUpdate(CREATE_ENTRY_TABLE_SQL_IDX2);
        m_db->ExecuteUpdate(CREATE_ENTRY_TABLE_SQL_IDX3);
        m_db->ExecuteUpdate(CREATE_ENTRY_TABLE_SQL_IDX4);
        m_db->ExecuteUpdate(CREATE_ENTRY_TABLE_SQL_IDX5);
        
        // Func Arg Table
        m_db->ExecuteUpdate(CREATE_FUNC_ARGS_SQL);
        m_db->ExecuteUpdate(CREATE_FUNC_ARGS_SQL_IDX1);
        
        // Files Table
        m_db->ExecuteUpdate(CREATE_FILES_TABLE_SQL);
        m_db->ExecuteUpdate(CREATE_FILES_TABLE_SQL_IDX1);
        
    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
}

void PHPStorage::DoStore(PHPEntry& entry, int parentDbID)
{
    entry.save( m_db, parentDbID );
    
    std::vector<PHPFuncArg> &args = entry.getArguments();
    std::vector<PHPFuncArg>::iterator iter2 = args.begin();
    for(; iter2 != args.end(); ++iter2) {
        iter2->save( m_db, entry.getDbId() );
    }
    
    PHPEntry::Vector_t& entries = entry.getEntries();
    
    PHPEntry::Vector_t::iterator iter = entries.begin();
    for(; iter != entries.end(); ++iter) {
        PHPEntry &childEntry = *iter;
        DoStore( childEntry, entry.getDbId() );
    }
}

void PHPStorage::Rollback()
{
    try {
        m_db->Rollback();
    } catch (wxSQLite3Exception &e) {
        wxUnusedVar(e);
    }
}


void PHPStorage::DoUpdateFile(const wxString& filename)
{
    // Update the file timestamp
    try {
        
        wxSQLite3Statement st = m_db->PrepareStatement("REPLACE INTO FILES_TABLE (ID, FILE_NAME, LAST_UPDATED) VALUES ( NULL, ?, ?)" );
        st.Bind(1, filename);
        st.Bind(2, (int) time(NULL) );
        st.ExecuteUpdate();

    } catch ( wxSQLite3Exception &e ) {
        wxUnusedVar ( e );
    }
    
}

bool PHPStorage::FindByFullPath(const wxString& fullpath, PHPEntry& entry, size_t searchFlags)
{
    wxString real_name, real_ns;
    wxString path = fullpath;
    while ( path.Replace("\\\\", "\\") ) {}
    
    DoCorrectClassName(path, "", real_name, real_ns); 
    return PHPEntry::loadByNameAndNS( m_db, real_name, real_ns , entry, PHP_Kind_Unknown, searchFlags & FetchFlags_FullClass );
}

bool PHPStorage::FindEntryByNameAndNS(const wxString& name, const wxString& ns, PHPEntry& entry)
{
    return PHPEntry::loadByNameAndNS(m_db, name, ns, entry, PHP_Kind_Unknown, true);
}

bool PHPStorage::FindEntryByNameAndParentId(long parentDbID, const wxString& name, PHPEntry& entry)
{
    return PHPEntry::loadByParentIdAndName(m_db, parentDbID, name, PHP_Kind_Unknown, entry, true);
}

PHPEntry::Vector_t PHPStorage::FindEntriesByParentIdAndName(long parentDbID, const wxString &partname, size_t flags, int limit)
{
    wxUnusedVar( limit );
    return PHPEntry::loadByParentIdAndName(m_db, parentDbID, partname, flags & FetchFlags_PartMatch, flags & FetchFlags_FullClass);
}

PHPEntry::Vector_t PHPStorage::FindEntriesByParentId(long parentDbID, size_t flags, int limit)
{
    wxUnusedVar( limit );
    return PHPEntry::loadByParentId(m_db, parentDbID, flags);
}

void PHPStorage::OnWorkspaceClosed(wxCommandEvent& e)
{
    e.Skip();
    Close();
}

PHPEntry::Vector_t PHPStorage::FindSuperGloabls(const wxString& partname, size_t flags, int limit)
{
    wxUnusedVar( limit );
    return PHPEntry::FindSuperGloabls(m_db, partname);
}

PHPEntry::Vector_t PHPStorage::FindGloablFunctions(const wxString& partname, size_t flags, int limit)
{
    wxUnusedVar( limit );
    return PHPEntry::FindGloablFunctions(m_db, partname);
}

bool PHPStorage::FindEntryByNameAndParentId(long parentDbID, const wxString& name, PHPEntryKind kind, PHPEntry& entry)
{
    return PHPEntry::loadByParentIdAndName(m_db, parentDbID, name, kind, entry, true);
}

void PHPStorage::GetParents(const PHPEntry& cls, PHPEntry::LongVector_t& ids)
{
    PHPEntry::StringSet_t scannedParents;
    PHPEntry::GetParents(m_db, cls, ids, scannedParents);
}

void PHPStorage::OnWorkspaceRenamed(PHPEvent& e)
{
    e.Skip();
    Close();
    
    wxFileName tagsfile(e.GetFileName());
    tagsfile.SetExt(wxT("phptags"));
    OpenWorksace( tagsfile, true, true );
}

void PHPStorage::DeleteAllContent()
{
    try {

        // Delete all entries owned by this file name
        wxSQLite3Statement st2 = m_db->PrepareStatement("delete from ENTRY_TABLE");
        wxSQLite3Statement st3 = m_db->PrepareStatement("delete from FUNC_ARGS");
        wxSQLite3Statement st4 = m_db->PrepareStatement("delete from FILES_TABLE");

        st2.ExecuteUpdate();
        st3.ExecuteUpdate();
        st4.ExecuteUpdate();

    } catch (wxSQLite3Exception &e) {
        wxUnusedVar( e );
    }
}

void PHPStorage::QueueParsingRequest(const wxArrayString& files, bool full_retag)
{
    PHPParserThreadRequest* req = new PHPParserThreadRequest();
    req->bIsFullRetag           = full_retag;
    req->files                  = files;
    req->workspaceFile          = m_dbfile;
    PHPParserThread::Instance()->Add( req );
}
