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

private:
    void CreateSchema();
    void SplitFullname(const wxString& fullname, wxString& name, wxString& scope) const;
    PHPEntityBase::Ptr_t DoFindMemberOf(wxLongLong parentDbId, const wxString& exactName);

    void DoGetInheritanceParentIDs(PHPEntityBase::Ptr_t cls,
                                   std::vector<wxLongLong>& parents,
                                   std::set<wxLongLong>& parentsVisited);

    PHPEntityBase::Ptr_t DoFindScope(const wxString& fullname, ePhpScopeType scopeType = kPhpScopeTypeAny);
    PHPEntityBase::Ptr_t DoFindScope(wxLongLong id, ePhpScopeType scopeType = kPhpScopeTypeAny);

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
     * @brief save source file into the database
     */
    void UpdateSourceFile(PHPSourceFile& source);
};

#endif // PHPLOOKUPTABLE_H
