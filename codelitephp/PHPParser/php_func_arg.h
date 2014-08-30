#ifndef PHP_FUNC_ARG_H
#define PHP_FUNC_ARG_H

#include <wx/string.h>
#include <wx/wxsqlite3.h>
#include "php_parser_constants.h"
#include <wx/wxcrtvararg.h>

class PHPFuncArg
{
    long     dbId;
    long     entryId;
    wxString name;
    wxString type;
    int      lineNumber;
    size_t   attributes;
    wxString filename;
    size_t   index;
public:
    typedef std::vector<PHPFuncArg> Vector_t;
    
public:
    /**
     * @brief load self from database result set
     * @param res
     */
    PHPFuncArg(wxSQLite3ResultSet &res);
    PHPFuncArg();
    ~PHPFuncArg() {}
    static PHPFuncArg::Vector_t loadByEntryId(wxSQLite3Database* db, long entryDbId);
    
    /**
     * @brief store self to the database
     * @param db
     */
    void save(wxSQLite3Database *db, int entryId);
    
    void setIndex(size_t index) {
        this->index = index;
    }
    size_t getIndex() const {
        return index;
    }
    void setFilename(const wxString& filename) {
        this->filename = filename;
    }
    const wxString& getFilename() const {
        return filename;
    }
    void setDbId(long dbId) {
        this->dbId = dbId;
    }
    long getDbId() const {
        return dbId;
    }
    void setLineNumber(int lineNumber) {
        this->lineNumber = lineNumber;
    }
    int getLineNumber() const {
        return lineNumber;
    }
    void setName(const wxString& name) {
        this->name = name;
    }
    void setType(const wxString& type) {
        this->type = type;
    }
    size_t getAttributes() const {
        return attributes;
    }
    const wxString& getName() const {
        return name;
    }
    wxString getType(const wxString &doc = wxEmptyString) const ;

    void addAttribute(PHPModifier attr) {
        if(attr == PHP_Access_Protected || attr == PHP_Access_Public || attr == PHP_Access_Private)
            this->attributes &= ~(PHP_Access_Protected|PHP_Access_Public|PHP_Access_Private);
        this->attributes |= attr;
    }
    void setAttributes(size_t attr) {
        this->attributes = attr;
    }

    void print() const {
        wxPrintf(wxT("---- Argument ----\n"));
        wxPrintf(wxT("name      : %s\n"), name.c_str());
        wxPrintf(wxT("type      : %s\n"), type.c_str());
        wxPrintf(wxT("line      : %d\n"), lineNumber);
        wxPrintf(wxT("Attributes: %s\n"), GetAttributes(attributes).c_str());
    }
};


#endif // PHP_FUNC_ARG_H
