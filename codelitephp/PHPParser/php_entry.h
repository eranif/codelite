#ifndef PHP_ENTRY_H
#define PHP_ENTRY_H

#include "php_func_arg.h"
#include <set>
#include <vector>
#include <stack>

class PHPEntry
{
public:
    class LowerThan {
    public:
        bool operator()(const PHPEntry& a, const PHPEntry& b) const {
            wxString a_name, b_name;
            a_name << a.getNameSpace() << a.getName();
            b_name << b.getNameSpace() << b.getName();
            return a_name < b_name;
        }
    };
    
    class LowerThanByDbId {
    public:
        bool operator()(const PHPEntry& a, const PHPEntry& b) const {
           return a.getDbId() < b.getDbId();
        }
    };
    
    class Equal {
    public:
        bool operator()(const PHPEntry& a, const PHPEntry& b) const {
            wxString a_name, b_name;
            a_name << a.getNameSpace() << a.getName();
            b_name << b.getNameSpace() << b.getName();
            return a_name == b_name;
        }
    };
    
    class Predicate {
    public:
        bool operator()(const PHPEntry& a, const PHPEntry& b) const {
            return a.getDbId() == b.getDbId();
        }
    };
    
public:
    typedef std::vector<long>  LongVector_t;
    typedef std::set<wxString> StringSet_t;
    typedef std::list<PHPEntry> List_t;
    typedef std::stack<PHPEntry> Stack_t;
    
protected:
    long                         dbId;
    long                         parentDbId;
    PHPEntryKind                 kind;
    wxString                     name;
    wxString                     type;
    wxString                     typeScope;
    wxString                     docComment;
    size_t                       attributes;
    wxString                     defaultValue;
    std::vector<PHPFuncArg>      arguments;
    wxString                     nameSpace;
    wxString                     parent;
    std::map<wxString, PHPEntry> locals;
    int                          lineNumber;
    int                          terminatingOperator;
    wxString                     fileName;
    int                          prefixOperator;
    // Class related
    wxString                   inherits;    // no multiple inheritance in PHP
    wxArrayString              usingTraits; // vector of traits we are using
    wxArrayString              implements;  // vector of interfaces we implements
    std::vector<PHPEntry>      entries;
    std::map<wxString, size_t> entriesMap;
    
public:
    static void GetParents(wxSQLite3Database* db, const wxString& name, const wxString& nameSpace, PHPEntry::LongVector_t &ids, PHPEntry::StringSet_t &scannedParents);
    static void GetParents(wxSQLite3Database* db, const PHPEntry& cls, PHPEntry::LongVector_t& ids, PHPEntry::StringSet_t& scannedParents);
    static void GetParents(wxSQLite3Database* db, long dbId, PHPEntry::LongVector_t& ids, PHPEntry::StringSet_t& scannedParents);

public:
    typedef std::vector<PHPEntry> Vector_t;
    typedef std::vector<PHPEntry*> VectorPtr_t;
    
    wxString FormatDoxygenComment() const;
    
    wxString getTypeInfoTooltip() const;
    bool isClass() const {
        return kind == PHP_Kind_Class;
    }
    
    bool isNamespace() const {
        return kind == PHP_Kind_Namespace;
    }
    
    bool isFunction() const {
        return kind == PHP_Kind_Function;
    }
    
    bool isStatic() const {
        return attributes & PHP_Attr_Static;
    }
    bool isReference() const {
        return attributes & PHP_Attr_Reference;
    }
    
    /**
     * @brief prepend $ to the name if needed ( i.e. isStatic() && isMember() ) or remove it
     */
    void FixDollarPrefix();
    
    bool isMember() const {
        return kind == PHP_Kind_Member;
    }
    
    bool isVariable() const {
        return kind == PHP_Kind_Variable;
    }
    
    bool isScope() const {
        return kind == PHP_Kind_Class || kind == PHP_Kind_Namespace;
    }
    
    wxString makeFullPath(const wxString &curns) const;
public:
    /**
     * @brief construct a class entry
     * @param classname
     */
    PHPEntry(const wxString& classname);
    PHPEntry();
    ~PHPEntry() {}
    
    bool operator<(const PHPEntry& other) const {
        return this->getDbId() < other.getDbId();
    }
    
    void addInterface(const wxString &iface) ;
    
    void addTrait(const wxString &trait) {
        usingTraits.Add(trait);
    }
    
    const PHPFuncArg* findArgument(const wxString &name) const {
        for(size_t i=0; i<arguments.size(); ++i) {
            if ( arguments.at(i).getName() == name ) {
                return &arguments.at(i);
            }
        }
        return NULL;
    }
    
    PHPEntry* findChild(const wxString &name, PHPEntryKind kind) {
        wxString actualName = name;
        if(kind == PHP_Kind_Variable)
            actualName.Prepend(wxT("$"));

        std::map<wxString, size_t>::iterator iter = entriesMap.find(actualName);
        if(iter == entriesMap.end()) {
            return NULL;
        }
        return &entries.at(iter->second);
    }

    const PHPEntry* findChild(const wxString &name, PHPEntryKind kind) const {
        wxString actualName = name;
        if(kind == PHP_Kind_Variable && !actualName.StartsWith(wxT("$")))
            actualName.Prepend(wxT("$"));

        std::map<wxString, size_t>::const_iterator iter = entriesMap.find(actualName);
        if(iter == entriesMap.end()) {
            return NULL;
        }
        return &entries.at(iter->second);
    }
    
    /**
     * @brief return the full path to this entry (including namespaces etc)
     * the full path is always starting with \ (namespace operator)
     * it only make sense to call this function when the entry is of type container (class, namespace etc)
     * but not a function, constant variable etc.
     */
    wxString getContainerPath() const;
    void setAttributes(size_t attributes) {
        this->attributes = attributes;
    }
    void setEntries(const std::vector<PHPEntry>& entries) ;
    void setEntriesMap(const std::map<wxString, size_t>& entriesMap) {
        this->entriesMap = entriesMap;
    }
    void setImplements(const wxArrayString& implements) {
        this->implements = implements;
    }
    void setInherits(const wxString& inherits) ;
    void setLocals(const std::map<wxString, PHPEntry>& locals) {
        this->locals = locals;
    }
    void setParentDbId(long parentDbId) {
        this->parentDbId = parentDbId;
    }
    void setUsingTraits(const wxArrayString& usingTraits) {
        this->usingTraits = usingTraits;
    }
    const std::vector<PHPEntry>& getEntries() const {
        return entries;
    }
    std::vector<PHPEntry>& getEntries() {
        return entries;
    }
    const std::map<wxString, size_t>& getEntriesMap() const {
        return entriesMap;
    }
    const wxArrayString& getImplements() const {
        return implements;
    }
    const wxString& getInherits() const {
        return inherits;
    }
    long getParentDbId() const {
        return parentDbId;
    }
    const wxArrayString& getUsingTraits() const {
        return usingTraits;
    }
    
    // ============================================================================================================
    // db
    // ============================================================================================================
    static wxString DoMakeParentIdSql(wxSQLite3Database* db, long parentId);
    static PHPEntry::Vector_t loadByParentId(wxSQLite3Database* db, long parentDbId, bool includeChildren = false);
    static PHPEntry::Vector_t loadByParentId(wxSQLite3Database* db, long parentDbId, size_t flags);
    static PHPEntry::Vector_t loadByParentIdAndName(wxSQLite3Database* db, long parentDbId, const wxString &name, bool partialMatch = true, bool includeChildren = false);
    
    static bool loadByParentIdAndName(wxSQLite3Database* db, long parentDbId, const wxString &name, PHPEntryKind kind, PHPEntry& entry, bool includeChildren = false);
    static bool loadById(wxSQLite3Database* db, long dbId, PHPEntry& entry, bool includeChildren = false);
    static bool loadByNameAndNS(wxSQLite3Database* db, const wxString &name, const wxString &ns, PHPEntry& cls, PHPEntryKind kind = PHP_Kind_Unknown, bool includeChildren = false);
    static PHPEntry fromResultSet(wxSQLite3Database* db, wxSQLite3ResultSet &res, bool includeChildren);
    static PHPEntry::Vector_t loadByName(wxSQLite3Database* db, const wxString &filter, bool partialMatch = true, bool includeChildren = false);
    static PHPEntry::Vector_t FindGloablFunctions(wxSQLite3Database* db, const wxString& name);
    static PHPEntry::Vector_t FindSuperGloabls(wxSQLite3Database* db, const wxString& name);
    static PHPEntry::Vector_t LoadNamespaceEntries(wxSQLite3Database* db, const wxString& ns, const wxString &partname, size_t flags);
    static void DoAddLimitClause(wxString &sql);
    void save( wxSQLite3Database* db, long parentDbId );
    void setTypeScope(const wxString& typeScope) {
        this->typeScope = typeScope;
    }
    const wxString& getTypeScope() const {
        return typeScope;
    }

    void sortArgs();

    void setPrefixOperator(int prefixOperator) {
        this->prefixOperator = prefixOperator;
    }
    int getPrefixOperator() const {
        return prefixOperator;
    }
    void setFileName(const wxString& fileName) {
        this->fileName = fileName;
    }
    const wxString& getFileName() const {
        return fileName;
    }
    void setDbId(long dbId) {
        this->dbId = dbId;
    }
    long getDbId() const {
        return dbId;
    }
    void setArguments(const std::vector<PHPFuncArg>& arguments) {
        this->arguments = arguments;
    }
    const std::vector<PHPFuncArg>& getArguments() const {
        return arguments;
    }
    std::vector<PHPFuncArg>& getArguments() {
        return arguments;
    }
    void setTerminatingOperator(int terminatingOperator) {
        this->terminatingOperator = terminatingOperator;
    }
    int getTerminatingOperator() const {
        return terminatingOperator;
    }
    bool isOk() const {
        return name.IsEmpty() == false && lineNumber != -1;
    }

    bool isGlobal() const {
        return (this->parent == wxT("<global>")) && (this->nameSpace.IsEmpty());
    }

    void setParent(const wxString& parent) {
        this->parent = parent;
    }
    
    const wxString& getParent() const {
        return parent;
    }
    
    void setNameSpace(const wxString& nameSpace) {
        this->nameSpace.Clear();
        if ( !nameSpace.StartsWith("\\") && !nameSpace.IsEmpty() ) {
            this->nameSpace = "\\";
        }
        this->nameSpace << nameSpace;
    }
    
    const wxString& getNameSpace() const {
        return nameSpace;
    }
    void print() const {
        wxPrintf(wxT("--------------\n"));
        switch(kind) {
        case PHP_Kind_Constant:
            wxPrintf(wxT("Kind      : PHP_Kind_Constant\n"));
            break;
        case PHP_Kind_Member:
            wxPrintf(wxT("Kind      : PHP_Kind_Member\n"));
            break;
        case PHP_Kind_Variable:
            wxPrintf(wxT("Kind      : PHP_Kind_Variable\n"));
            break;
        case PHP_Kind_Function:
            wxPrintf(wxT("Kind      : PHP_Kind_Function\n"));
            break;
        case PHP_Kind_This:
            wxPrintf(wxT("Kind      : PHP_Kind_This\n"));
            break;
        default:
            break;
        }
        wxPrintf(wxT("name      : %s\n"), name.c_str());
        wxPrintf(wxT("line      : %d\n"),  getLineNumber());
        wxPrintf(wxT("parent    : %s\n"), parent.c_str());
        wxPrintf(wxT("namespace : %s\n"), nameSpace.c_str());
        wxPrintf(wxT("init value: %s\n"), defaultValue.c_str());
        wxPrintf(wxT("type      : %s\n"), type.c_str());
        wxPrintf(wxT("Ends with : %d\n"),  terminatingOperator);
        
        wxPrintf("Children : \n");
        PHPEntry::Vector_t::const_iterator it = entries.begin();
        for( ; it != entries.end(); ++it ) {
            it->print();
        }
        
        if(docComment.IsEmpty() == false) {
            wxPrintf(wxT("doc       :\n%s\n"), docComment.c_str());
        }
        wxPrintf(wxT("Attributes: %s\n"), GetAttributes(attributes).c_str());
        if(arguments.empty() == false) {
            wxPrintf(wxT("Func args:\n"));
            std::vector<PHPFuncArg>::const_iterator iter = arguments.begin();
            for(; iter != arguments.end(); iter++) {
                (*iter).print();
            }
        }

        if(locals.empty() == false) {
            wxPrintf(wxT("Local variables:\n"));
            std::map<wxString, PHPEntry>::const_iterator iter = locals.begin();
            for(; iter != locals.end(); iter++) {
                iter->second.print();
            }
        }
    }

    void reset() {
        kind = PHP_Kind_Variable;
        this->name = "";
        type.Clear();
        docComment.Clear();
        attributes = PHP_Attr_None;
        defaultValue.Clear();
        arguments.clear();
        nameSpace.Clear();
        parent.Clear();
        lineNumber = -1;
        locals.clear();
        terminatingOperator = 0;
        fileName.Clear();
        inherits.Clear();
        implements.Clear();
        entries.clear();
        entriesMap.clear();
        usingTraits.Clear();
        prefixOperator = 0;
    }
    
    bool isGlobalScopeClass() const {
        return name == "<global>";
    }
    
    void addEntry(const PHPEntry& entry) {
        entries.push_back(entry);
        // keep the array index in the map for fast searches
        entriesMap.insert(std::make_pair(entry.getName(), entries.size() -1));
    }

    std::map<wxString, PHPEntry> getLocals() const;
    void setLineNumber(int lineNumber) {
        this->lineNumber = lineNumber;
    }
    int getLineNumber() const {
        return lineNumber;
    }
    void addNewLocal(const wxString &name, const wxString &type = wxT(""), PHPModifier attr = PHP_Attr_None);

    void newArgument(const wxString &name) ;
    void addArgument(const PHPFuncArg &arg);

    PHPFuncArg& getCurrentArg() {
        if(arguments.empty() == false) {
            return arguments.back();
        }
        static PHPFuncArg dummy;
        return dummy;
    }

    void setDefaultValue(const wxString& defaultValue) {
        this->defaultValue = defaultValue;
    }
    const wxString& getDefaultValue() const {
        return defaultValue;
    }
    void setDocComment(const wxString& docComment) {
        this->docComment = docComment;
    }
    void setKind(const PHPEntryKind& kind) {
        this->kind = kind;
    }
    wxString& getDocComment() {
        return docComment;
    }
    const wxString& getDocComment() const {
        return docComment;
    }
    const PHPEntryKind& getKind() const {
        return kind;
    }
    void addAttribute(PHPModifier attr) {
        if(attr == PHP_Access_Protected || attr == PHP_Access_Public || attr == PHP_Access_Private)
            this->attributes &= ~(PHP_Access_Protected|PHP_Access_Public|PHP_Access_Private);

        this->attributes |= attr;
    }
    void setName(const wxString& name);
    void setType(const wxString& type) {
        this->type = type;
    }
    size_t getAttributes() const {
        return attributes;
    }
    const wxString& getName() const {
        return name;
    }
    wxString getType() const ;
    wxString getSignature() const;
    static void CorrectClassName(const wxString& in_name, const wxString& in_ns, wxString& out_name, wxString& out_ns);

};
#endif // PHP_ENTRY_H
