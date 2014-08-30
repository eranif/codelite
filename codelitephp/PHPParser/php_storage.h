#ifndef PHPSTORAGE_H
#define PHPSTORAGE_H


#include <wx/filename.h>
#include <wx/string.h>
#include <wx/thread.h>
#include <wx/wxsqlite3.h>
#include <set>
#include <wx/event.h>
#include "php_event.h"
#include "php_parser_api.h" // Contains the definition of all the basics
#include <cl_command_event.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ResourceItem {
    PHPEntryKind type;
    wxString     displayName;
    wxFileName   filename;
    int          line;
    wxString     name;
    ResourceItem() {}
    ResourceItem(const PHPEntry& entry) {
        type        = entry.getKind();
        displayName = entry.getName();
        filename    = entry.getFileName();
        line        = entry.getLineNumber();
        name        = entry.getName();
    }

    wxString typeAsString() const {
        switch(type) {
        case PHP_Kind_Function:
            return wxT("Function");
        case PHP_Kind_Constant:
            return wxT("Constant");
        case PHP_Kind_Class:
            return wxT("Class");
        case PHP_Kind_Namespace:
            return wxT("Namespace");
        case PHP_Kind_File:
            return wxT("File");
        case PHP_Kind_Member:
            return wxT("Member");
        default:
        case PHP_Kind_Variable:
            return wxT("Variable");
        }
    }
};

typedef std::vector<ResourceItem> ResourceVector_t;

//--------------------------------------------------------------------------------------//
// TABLES:                                                                              //
//--------------------------------------------------------------------------------------//
// CLASS_TABLE
// ===========
// ID INETGER NOT NULL PRIMARY AUTO_INCREMENT,
// NAME TEXT,
// NAMESPACE TEXT NOT NULL DEFAULT "",
// ATTRIBUTES INTEGER NOT NULL DEFAULT 0,
// INHERITS TEXT,
// USING_TRAITS TEXT,
// IMPLEMENTS TEXT,
// LINE_NUMER INTEGER NOT NULL DEFAULT 0,
// FILE_NAME TEXT

// Index
// =====
// CREATE INDEX IF NOT EXIST CLASS_TABLE_IDX_1 ON CLASS_TABLE(NAME)
// CREATE UNIQUE INDEX IF NOT EXIST CLASS_TABLE_IDX_2 ON CLASS_TABLE(NAME, NAMESPACE)
// CREATE INDEX IF NOT EXIST CLASS_TABLE_IDX_3 ON CLASS_TABLE(LINE_NUMER)
// CREATE INDEX IF NOT EXIST CLASS_TABLE_IDX_3 ON CLASS_TABLE(FILE_NAME)

// ENTRY_TABLE
// ===========
// ID INTEGER NOT NULL PRIMARY AUTO_INCREMENT,
// PARENT_ID INTEGER NOT NULL,
// NAME TEXT,
// INTEGER KIND,
// TYPE TEXT,
// TYPE_SCOPE TEXT,
// DOC_COMMENT TEXT,
// ATTRIBUTES INTEGER NOT NULL DEFAULT 0,
// DEFAULT_VALUE TEXT NOT NULL DEFAULT "",
// TEXT NAMESPACE NOT NULL DEFAULT "",
// INTEGER LINE_NUMBER NOT NULL DEFAULT 0,
// FILE_NAME TEXT

// Index
// =====
// CREATE INDEX IF NOT EXIST ENTRY_TABLE_IDX_1        ON ENTRY_TABLE(NAME)
// CREATE UNIQUE INDEX IF NOT EXIST ENTRY_TABLE_IDX_2 ON ENTRY_TABLE(NAME, PARENT_ID)
// CREATE INDEX IF NOT EXIST ENTRY_TABLE_IDX_3        ON ENTRY_TABLE(PARENT_ID)
// CREATE INDEX IF NOT EXIST ENTRY_TABLE_IDX_4        ON ENTRY_TABLE(FILE_NAME)
// CREATE INDEX IF NOT EXIST ENTRY_TABLE_IDX_5        ON ENTRY_TABLE(LINE_NUMBER)

// FUNC_ARGS
// =========
// ID INTEGER NOT NULL PRIMARY AUTO_INCREMENT,
// ENTRY_ID INTEGER NOT NULL,
// NAME TEXT,
// TYPE TEXT,
// LINE_NUMER INTEGER NOT NULL DEFAULT 0,
// ATTRIBUTES INTEGER NOT NULL DEFAULT 0,
// FILE_NAME TEXT,
// ARG_INDEX INTEGER NOT NULL DEFAULT 0

// Index
// =====
// CREATE INDEX IF NOT EXIST FUNC_ARGS_IDX_1 ON FUNC_ARGS(ENTRY_ID)

// FILES_TABLE
// =========
// ID INTEGER NOT NULL PRIMARY AUTO_INCREMENT,
// FILE_NAME TEXT,
// LAST_UPDATED INTEGER

// Index
// =====
// CREATE INDEX IF NOT EXIST FILES_TABLE_IDX_1 ON FILES_TABLE(FILE_NAME)


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class PHPParserThread;
class PHPStorage : public wxEvtHandler
{
    static PHPStorage* ms_instance;
    wxSQLite3Database* m_db;
    wxString           m_dbfile;
    wxCriticalSection  m_cs;
    size_t             m_limit;
    
    friend class PHPParserThread;
    
public:
    // Search Flags
    enum {
        FetchFlags_ExactMatch    = 0x00000001, // Exact name matching
        FetchFlags_PartMatch     = 0x00000002, // partial name matching
        FetchFlags_FullClass     = 0x00000004, // classes includes their members entries
        FetchFlags_FunctionsOnly = 0x00000008, // fetch functions only
        FetchFlags_Self          = 0x00000010, // Fetch for 'self' keyword
        FetchFlags_Static        = 0x00000020, // Fetch for the 'static' keyword
        FetchFlags_Self_All      = 0x00000040, // Fetch all the children of this class, but NOT its parents
    };

public:
    /**
     * @class AutoCommitter
     * @brief a helper class that starts/commits transcation automatically
     */
    class AutoCommitter
    {
        PHPStorage* m_storage;
        bool        m_autoCommit;
    public:
        AutoCommitter(PHPStorage* storage, bool autoCommit) : m_storage(storage), m_autoCommit(autoCommit) {
            if ( m_storage && m_autoCommit ) {
                m_storage->Begin();
            }
        }
        ~AutoCommitter() {
            if ( m_storage && m_autoCommit ) {
                m_storage->Commit();
            }
        }
    };

protected:
    /**
     * @brief an internal functions used only by the parser thread!
     */
    void ParseAndStoreFileNoLock(const wxString &filename, bool commit, bool deleteBeforeInsert = true);
    void CreateSchema();
    void DoStore(PHPEntry& entry, int parentDbID);
    void DoUpdateFile(const wxString& filename);
    void DoGetParents(const wxString& name, const wxString& nameSpace, std::vector<long> &ids, std::set<wxString> &scannedParents);
    void DoGetParents(const PHPEntry& cls, std::vector<long> &ids, std::set<wxString> &scannedParents);
    void DoCorrectClassName(const wxString &in_name, const wxString &in_ns, wxString &out_name, wxString &out_ns);

    /**
     * @brief use the additional include path to get a list of all files
     */
    void DoGetExternalFiles(wxArrayString &files);

    /**
     * @brief return list of files that are older than their actual timestamp on the disk
     */
    void DoFilterFilesThatDontNeedReparsing(const wxArrayString &files, wxArrayString &filesNeedsReparsing);
    time_t DoGetFileLastUpdatedTimestamp(const wxString &filename);
    void DoReparseFiles(const wxArrayString &files, bool full_retag, bool deleteBeforeInsert);
    void QueueParsingRequest(const wxArrayString& files, bool full_retag);
    
    // Event handlers
    void OnFileAdded(clCommandEvent& event);
    void OnFilesRemoved(clCommandEvent &event);
    void OnFileSaved(clCommandEvent &e);
    void OnRetagWorkspace(wxCommandEvent &e);
    void OnRetagWorkspaceFull(wxCommandEvent &e);
    void OnWorkspaceClosed(wxCommandEvent &e);
    void OnWorkspaceRenamed(PHPEvent &e);

public:
    void ConnectEvents();
    void OpenWorksace(const wxFileName &dbfile, bool retagOnOpen = true, bool fullRetag = false);
    void Close();
    bool IsOpen() const;

    /////////////////////////////////////////
    // Public API
    /////////////////////////////////////////

    void SetLimit(size_t limit) {
        this->m_limit = limit;
    }
    size_t GetLimit() const {
        return m_limit;
    }
    void Begin();
    void Commit();
    void Rollback();
    void DeleteByFile(const wxFileName& filename);
    void DeleteAllContent();
    void ParseFile(const wxString &filename, bool commit, bool deleteBeforeInsert = true);
    void ReparseFiles(bool full_retag);

    /**
     * @brief
     * @param name
     * @param ns
     * @param entry
     */
    bool FindEntryByNameAndNS( const wxString &name, const wxString &ns, PHPEntry& entry);

    /**
     * @brief find an entry by a parent DbID and a name
     */
    bool FindEntryByNameAndParentId( long parentDbID, const wxString &name, PHPEntry& entry);

    /**
     * @brief find an entry by a parent DbID and a name
     */
    bool FindEntryByNameAndParentId( long parentDbID, const wxString &name, PHPEntryKind kind, PHPEntry& entry);

    /**
     * @brief load all children of parentDbID. Limit the return value to 'limit'
     */
    PHPEntry::Vector_t FindEntriesByParentIdAndName(long parentDbID, const wxString &partname, size_t flags, int limit = 500);

    /**
     * @brief load all global functions matching 'partname'
     */
    PHPEntry::Vector_t FindGloablFunctions(const wxString &partname, size_t flags, int limit = 500);
    
    /**
     * @brief load all superglobals
     */
    PHPEntry::Vector_t FindSuperGloabls(const wxString &partname, size_t flags, int limit = 500);

    /**
     * @brief load all children of parentDbID. Limit the return value to 'limit'
     */
    PHPEntry::Vector_t FindEntriesByParentId(long parentDbID, size_t flags, int limit = 500);

    /**
     * @brief fetch classes from the database
     */
    void FetchClasses(const wxString &name, const wxString &nameSpace, std::vector<PHPEntry> &classes, size_t searchFlags = FetchFlags_PartMatch);

    /**
     * @brief return a string array of all namespaces entries
     */
    PHPEntry::Vector_t FetchNamespaceEntries(const wxString& nameSpace, const wxString &partName, size_t flags = FetchFlags_PartMatch);

    bool FindClass(const wxString &name, const wxString &nameSpace, size_t searchFlags, PHPEntry &cls);

    /**
     * @brief get a full list of classes from the database (limit to 500)
     */
    bool GetAllResources(ResourceVector_t& classes, const wxString &filter);

    /**
     * @brief find a symbol (a symbol can be: class, function or constant)
     * @param name symbol name
     * @param items [output]
     */
    void FindSymbol(const wxString &name, std::vector<ResourceItem>& items);

    /**
     * @brief find an entry in the database with a given full path (ns + name)
     */
    bool FindByFullPath(const wxString &fullpath, PHPEntry& entry, size_t searchFlags = FetchFlags_ExactMatch|FetchFlags_FullClass);

    void GetParents(const PHPEntry& cls, PHPEntry::LongVector_t& ids);

public:
    static PHPStorage* Instance();
    static void Release();

private:
    PHPStorage();
    virtual ~PHPStorage();
};

#endif // PHPSTORAGE_H
