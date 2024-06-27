#ifndef PHPDOCVAR_H
#define PHPDOCVAR_H

#include "PHPSourceFile.h"
#include "codelite_exports.h"

#include <list>
#include <map>
#include <memory>
#include <wx/string.h>
#include <wx/wxsqlite3.h>

class WXDLLIMPEXP_CL PHPDocVar
{
    bool m_isOk;
    wxString m_name;
    wxString m_type;
    wxLongLong m_dbId;
    wxLongLong m_parentDbId;
    wxFileName m_filename;
    int m_lineNumber;

private:
    void Parse(PHPSourceFile& sourceFile, const wxString& doc);

public:
    using Ptr_t = std::shared_ptr<PHPDocVar>;
    using List_t = std::list<PHPDocVar::Ptr_t>;
    using Map_t = std::map<wxString, PHPDocVar::Ptr_t>;

public:
    PHPDocVar(PHPSourceFile& sourceFile, const wxString& doc);
    PHPDocVar();
    virtual ~PHPDocVar();

    bool IsOk() const { return m_isOk; }

    void SetName(const wxString& name) { this->m_name = name; }
    void SetType(const wxString& type) { this->m_type = type; }

    const wxString& GetName() const { return m_name; }
    const wxString& GetType() const { return m_type; }

    void SetDbId(const wxLongLong& dbId) { this->m_dbId = dbId; }
    const wxLongLong& GetDbId() const { return m_dbId; }

    void SetParentDbId(const wxLongLong& parentDbId) { this->m_parentDbId = parentDbId; }
    const wxLongLong& GetParentDbId() const { return m_parentDbId; }

    void SetFilename(const wxFileName& filename) { this->m_filename = filename; }
    void SetIsOk(bool isOk) { this->m_isOk = isOk; }
    void SetLineNumber(int lineNumber) { this->m_lineNumber = lineNumber; }
    const wxFileName& GetFilename() const { return m_filename; }
    int GetLineNumber() const { return m_lineNumber; }

    // Database API
    void Store(wxSQLite3Database& db, wxLongLong parentDdId);
    void FromResultSet(wxSQLite3ResultSet& res);
};

#endif // PHPDOCVAR_H
