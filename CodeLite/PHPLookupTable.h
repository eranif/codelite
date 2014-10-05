#ifndef PHPLOOKUPTABLE_H
#define PHPLOOKUPTABLE_H

#include "codelite_exports.h"
#include <wx/string.h>
#include "PHPEntityBase.h"
#include "wx/wxsqlite3.h"
#include "PHPSourceFile.h"
#include <vector>
#include <set>
#include <wx/longlong.h>
#include "cl_command_event.h"

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPHP_PARSE_STARTED, clParseEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPHP_PARSE_ENDED, clParseEvent);
wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CL, wxPHP_PARSE_PROGRESS, clParseEvent);

enum ePhpScopeType {
    kPhpScopeTypeAny = -1,
    kPhpScopeTypeNamespace = 0,
    kPhpScopeTypeClass = 1,
};

/**
 * @class PHPLookupTable
 * @brief a database lookup class which allows a quick access
 * to all entities parsed in the workspace
 */
class WXDLLIMPEXP_CL PHPLookupTable
{
    wxSQLite3Database m_db;
    size_t m_sizeLimit;

public:
    enum eLookupFlags {
        kLookupFlags_None = 0,
        kLookupFlags_ExactMatch = (1 << 1),
        kLookupFlags_Contains = (1 << 2),
        kLookupFlags_StartsWith = (1 << 3),
        kLookupFlags_Members = (1 << 4),           // Class members
        kLookupFlags_Constants = (1 << 5),         // 'const'
        kLookupFlags_StaticMembers = (1 << 6),     // include static members/functions (static::, class_type::)
        kLookupFlags_SelfStaticMembers = (1 << 7), // Current class static members only (self::)
    };
    
    enum eUpdateMode {
        kUpdateMode_Fast,
        kUpdateMode_Full,
    };
private:
    void DoAddNameFilter(wxString& sql, const wxString& nameHint, size_t flags);

    void CreateSchema();
    void SplitFullname(const wxString& fullname, wxString& name, wxString& scope) const;
    PHPEntityBase::Ptr_t DoFindMemberOf(wxLongLong parentDbId, const wxString& exactName);

    void DoGetInheritanceParentIDs(PHPEntityBase::Ptr_t cls,
                                   std::vector<wxLongLong>& parents,
                                   std::set<wxLongLong>& parentsVisited);

    PHPEntityBase::Ptr_t DoFindScope(const wxString& fullname, ePhpScopeType scopeType = kPhpScopeTypeAny);
    PHPEntityBase::Ptr_t DoFindScope(wxLongLong id, ePhpScopeType scopeType = kPhpScopeTypeAny);
    /**
     * @brief sqlite uses '_' as a wildcard. escape it using '^'
     * as our escape char
     */
    wxString EscapeWildCards(const wxString& str);

    void DoAddLimit(wxString& sql);

    /**
     * @brief allocate new entity that matches the table name
     */
    PHPEntityBase::Ptr_t NewEntity(const wxString& tableName, ePhpScopeType scopeType = kPhpScopeTypeAny);

    /**
     * @brief load entities from table and name hint
     * if nameHint is empty, return empty list
     */
    void LoadFromTableByNameHint(PHPEntityBase::List_t& matches,
                                 const wxString& tableName,
                                 const wxString& nameHint,
                                 eLookupFlags flags);

    bool CollectingStatics(size_t flags) const
    {
        return flags & (kLookupFlags_SelfStaticMembers | kLookupFlags_StaticMembers);
    }

    /**
     * @brief return children of parentId _WITHOUT_ taking inheritance into consideration
     */
    void DoFindChildren(PHPEntityBase::List_t& matches,
                        wxLongLong parentId,
                        size_t flags = kLookupFlags_None,
                        const wxString& nameHint = "");
    
    /**
     * @brief return the timestamp of the last parse for 'filename'
     */
    wxLongLong GetFileLastParsedTimestamp(const wxFileName& filename);
    
    /**
     * @brief update the file's last updated timestamp
     */
    void UpdateFileLastParsedTimestamp(const wxFileName& filename);
public:
    PHPLookupTable();
    virtual ~PHPLookupTable();

    /**
     * @brief open the lookup table database
     */
    void Open(const wxString& workspacePath);

    /**
     * @brief
     * @return
     */
    bool IsOpened() const;

    /**
     * @brief close the lookup table database
     */
    void Close();

    /**
     * @brief find a scope symbol (class or namespace) by its fullname
     */
    PHPEntityBase::Ptr_t FindScope(const wxString& fullname);
    /**
     * @brief find a class with a given name
     */
    PHPEntityBase::Ptr_t FindClass(const wxString& fullname);

    /**
     * @brief find a class with a given database ID
     */
    PHPEntityBase::Ptr_t FindClass(wxLongLong id);

    /**
     * @brief find a member of parentDbId with name that matches 'exactName'
     */
    PHPEntityBase::Ptr_t FindMemberOf(wxLongLong parentDbId, const wxString& exactName);

    /**
     * @brief find children of a scope by its database ID.
     */
    PHPEntityBase::List_t
    FindChildren(wxLongLong parentId, size_t flags = kLookupFlags_None, const wxString& nameHint = "");

    /**
     * @brief load list of entities from all the tables that matches 'nameHint'
     * if nameHint is empty, return an empty list
     */
    void LoadAllByFilter(PHPEntityBase::List_t& matches,
                         const wxString& nameHint,
                         eLookupFlags flags = kLookupFlags_Contains);
    /**
     * @brief save source file into the database
     */
    void UpdateSourceFile(PHPSourceFile& source, bool autoCommit = true);

    /**
     * @brief update list of source files
     */
    void UpdateSourceFiles(const wxArrayString& files, eUpdateMode updateMode, bool parseFuncBodies = true);

    /**
     * @brief delete all entries belonged to filename.
     * @param filename the file name
     * @param autoCommit when true, issue a begin/commit transcation commands
     */
    void DeleteFileEntries(const wxFileName& filename, bool autoCommit = true);
    
    /**
     * @brief load function arguments from the database
     * @param parentId the function database ID
     */
    PHPEntityBase::List_t LoadFunctionArguments(wxLongLong parentId);
};

#endif // PHPLOOKUPTABLE_H
