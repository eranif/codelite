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


wxDECLARE_EVENT(wxPHP_PARSE_STARTED, clParseEvent);
wxDECLARE_EVENT(wxPHP_PARSE_ENDED, clParseEvent);
wxDECLARE_EVENT(wxPHP_PARSE_PROGRESS, clParseEvent);

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
        kLookupFlags_PartialMatch = (1 << 2),
    };

private:
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
    wxString EscapeWildCards(const wxString &str);
    
    void DoAddLimit(wxString& sql);
public:
    PHPLookupTable();
    virtual ~PHPLookupTable();

    /**
     * @brief open the lookup table database
     */
    void Open(const wxString& workspacePath);

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
    FindChildren(wxLongLong parentId, eLookupFlags flags = kLookupFlags_None, const wxString& nameHint = "");

    /**
     * @brief save source file into the database
     */
    void UpdateSourceFile(PHPSourceFile& source, bool autoCommit = true);
    
    /**
     * @brief update list of source files
     */
    void UpdateSourceFiles(const wxArrayString& files, bool parseFuncBodies = true);
};

#endif // PHPLOOKUPTABLE_H
