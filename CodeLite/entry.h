//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2008 by Eran Ifrah
// file name            : entry.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
#ifndef CODELITE_ENTRY_H
#define CODELITE_ENTRY_H

#if wxUSE_GUI
#include <wx/treectrl.h>
#endif

#include "codelite_exports.h"
#include "macros.h"
#include "smart_ptr.h"

#include <map>
#include <vector>
#include <wx/string.h>

class TagEntry;
typedef SmartPtr<TagEntry> TagEntryPtr;
typedef std::vector<TagEntryPtr> TagEntryPtrVector_t;

#define KIND_CLASS "class"
#define KIND_ENUM "enum"
#define KIND_CLASS_ENUM "cenum"
#define KIND_ENUMERATOR "enumerator"
#define KIND_FUNCTION "function"
#define KIND_PROTOTYPE "prototype"
#define KIND_MEMBER "member"
#define KIND_NAMESPACE "namespace"
#define KIND_VARIABLE "variable"
#define KIND_UNION "union"
#define KIND_TYPEDEF "typedef"
#define KIND_MACRO "macro"
#define KIND_STRUCT "struct"
#define KIND_FILE "file"

// make it public
enum class eTagKind {
    TAG_KIND_UNKNOWN = -1,
    TAG_KIND_CLASS,
    TAG_KIND_STRUCT,
    TAG_KIND_NAMESPACE,
    TAG_KIND_UNION,
    TAG_KIND_ENUM,
    TAG_KIND_ENUMERATOR,
    TAG_KIND_CENUM, // class enum
    TAG_KIND_MEMBER,
    TAG_KIND_VARIABLE,
    TAG_KIND_MACRO,
    TAG_KIND_TYPEDEF,
    TAG_KIND_LOCAL,
    TAG_KIND_PARAMETER, // function parameter
    TAG_KIND_FUNCTION,
    TAG_KIND_PROTOTYPE,
    TAG_KIND_KEYWORD,
};

/**
 * TagEntry is a persistent object which is capable of storing and loading itself from
 * various inputs:
 * - tagEntry (ctags structure)
 *
 * It contains all the knowledge of storing and retrieving itself from the database
 *
 * \ingroup CodeLite
 * \version 1.0
 * first version
 *
 * \date 11-11-2006
 * \author Eran
 */
class WXDLLIMPEXP_CL TagEntry
{
private:
    enum eTagFlag {
        TAG_PROP_CONST = (1 << 0),
        TAG_PROP_VIRTUAL = (1 << 2),
        TAG_PROP_STATIC = (1 << 3),
        TAG_PROP_DEFAULT = (1 << 4),
        TAG_PROP_OVERRIDE = (1 << 5),
        TAG_PROP_DELETED = (1 << 6),
        TAG_PROP_INLINE = (1 << 7),
        TAG_PROP_PURE = (1 << 8),
        TAG_PROP_SCOPEDENUM = (1 << 9),
        TAG_PROP_AUTO_VARIABLE = (1 << 10),
        TAG_PROP_LAMBDA = (1 << 11),
    };

private:
    wxString m_path;    ///< Tag full path
    wxString m_file;    ///< File this tag is found
    int m_lineNumber;   ///< Line number
    wxString m_pattern; ///< A pattern that can be used to locate the tag in the file
    wxString m_kind;    ///< Member, function, class, typedef etc.
    wxString m_parent;  ///< Direct parent
#if wxUSE_GUI
    wxTreeItemId m_hti; ///< Handle to tree item, not persistent item
#endif
    wxString m_name;           ///< Tag name (short name, excluding any scope names)
    wxStringMap_t m_extFields; ///< Additional extension fields
    long m_id;
    wxString m_scope;
    size_t m_flags;     // This member is not saved into the database
    wxString m_comment; // This member is not saved into the database
    wxString m_template_definition;
    wxString m_tag_properties;
    size_t m_tag_properties_flags = 0; // bitwise eTagFlag
    eTagKind m_tag_kind = eTagKind::TAG_KIND_UNKNOWN;
    wxString m_assignment;

public:
    enum {
        Tag_No_Signature_Format = 0x00000001, // Do not attempt to format the signature. Use the GetSignature() as is
        Tag_No_Return_Value_Eval = 0x00000002 // Do not evaluate the return value. Use GetReturnValue() instead
    };

    // Used by std::for_each to copy elements which are constructors
    class ForEachCopyIfCtor
    {
        TagEntryPtrVector_t& m_matches;

    public:
        ForEachCopyIfCtor(TagEntryPtrVector_t& v)
            : m_matches(v)
        {
        }
        void operator()(TagEntryPtr tag)
        {
            if(tag->IsConstructor()) {
                m_matches.push_back(tag);
            }
        }
    };

    void set_extra_field(const wxString& name, const wxString& value);

public:
    void SetComment(const wxString& comment) { this->m_comment = comment; }
    const wxString& GetComment() const { return m_comment; }
    void SetFlags(size_t flags) { this->m_flags = flags; }
    size_t GetFlags() const { return m_flags; }
    /**
     * Default constructor.
     */
    TagEntry();

    void FromLine(const wxString& line);

    bool IsClassTemplate() const;
    wxString GetTemplateDefinition() const;

    // function properties
    bool is_func_virtual() const;
    bool is_func_default() const;
    bool is_func_override() const;
    bool is_func_deleted() const;
    bool is_func_inline() const;
    bool is_func_pure() const;
    bool is_const() const;
    bool is_static() const;
    bool is_scoped_enum() const;
    bool is_auto() const;
    bool is_lambda() const;
    const wxString& get_assigment() const { return m_assignment; }

    void SetTagProperties(const wxString& prop);
    const wxString& GetTagProperties() const { return m_tag_properties; }

    /**
     * @brief assuming this tag is a variable, return its rvalue
     * this is done by parsing the pattern
     */
    static wxString TypenameFromPattern(const TagEntry* tag);

    /**
     * @brief return true if this tag a local variable of type `auto`
     */
    static bool IsAuto(const TagEntry* tag);

    /**
     * @brief create a function signature (including return value + properties)
     * that could be used in a header file
     */
    wxString GetFunctionDeclaration() const;

    /**
     * @brief create a function signature that could be used in a definition file
     */
    wxString GetFunctionDefinition() const;

    /**
     * Copy constructor.
     */
    TagEntry(const TagEntry& rhs);

    /**
     * \param rhs Source to copy from (right hand side)
     * \return this
     */
    TagEntry& operator=(const TagEntry& rhs);

    /**
     * Compare two TagEntry objects.
     * \param rhs Right hand side
     * \return true if identical, false otherwise
     */
    bool operator==(const TagEntry& rhs);

    /**
     *	Destructor
     */
    virtual ~TagEntry();

    /**
     * @brief return if this tag entry is a function tempalte
     */
    bool IsTemplateFunction() const;

    /**
     * @brief is this a local variable?
     */
    bool IsLocalVariable() const;

    /**
     * Construct a TagEntry from values.
     * \param fileName File name
     * \param name Tag name
     * \param lineNumber Tag line number
     * \param pattern Pattern
     * \param kind Tag kind (class, struct, etc)
     * \param extFields Map of extenstion fields (key:value)
     * \param project Project name
     */
    void Create(const wxString& fileName, const wxString& name, int lineNumber, const wxString& pattern,
                const wxString& kind, wxStringMap_t& extFields);

    /**
     * Test if this entry has been initialised.
     * \return true if this tag entry has been initialised
     */
    const bool IsOk() const { return GetKind() != _T("<unknown>"); }

    /**
     * Test of this tag is a container (class, union, struct or namespace
     */
    const bool IsContainer() const;

    /**
     * @brief return true if this tag represents a constructor
     */
    bool IsConstructor() const;
    /**
     * @brief return true if this tag represents a destructor
     */
    bool IsDestructor() const;

    /**
     * @brief return true of the this tag is a function or prototype
     */
    bool IsMethod() const;

    bool IsFunction() const;
    bool IsPrototype() const;
    bool IsMacro() const;
    bool IsClass() const;
    bool IsStruct() const;
    bool IsScopeGlobal() const;
    bool IsTypedef() const;
    bool IsMember() const;
    bool IsNamespace() const;
    bool IsEnum() const;
    bool IsEnumClass() const;
    bool IsParameter() const;
    bool IsVariable() const;
    bool IsUnion() const;
    bool IsEnumerator() const;
    bool IsKeyword() const;

    //------------------------------------------
    // Operations
    //------------------------------------------
    int GetId() const { return m_id; }
    void SetId(int id) { m_id = id; }

    const wxString& GetName() const { return m_name; }
    void SetName(const wxString& name) { m_name = name; }

    const wxString& GetPath() const { return m_path; }
    void SetPath(const wxString& path) { m_path = path; }

    const wxString& GetFile() const { return m_file; }
    void SetFile(const wxString& file) { m_file = file; }

    int GetLine() const { return m_lineNumber; }
    void SetLine(int line) { m_lineNumber = line; }

    wxString GetPattern() const;
    /**
     * @brief return the entry pattern without the regex prefix/suffix
     */
    wxString GetPatternClean() const;

    void SetPattern(const wxString& pattern) { m_pattern = pattern; }

    wxString GetKind() const;
    void SetKind(const wxString& kind);

    const wxString& GetParent() const { return m_parent; }
    void SetParent(const wxString& parent) { m_parent = parent; }
#if wxUSE_GUI
    wxTreeItemId& GetTreeItemId() { return m_hti; }
    void SetTreeItemId(wxTreeItemId& hti) { m_hti = hti; }
#endif

    wxString GetAccess() const { return GetExtField(_T("access")); }
    void SetAccess(const wxString& access) { set_extra_field("access", access); }

    wxString GetSignature() const { return GetExtField(_T("signature")); }
    void SetSignature(const wxString& sig) { set_extra_field("signature", sig); }
    void SetInherits(const wxString& inherits) { set_extra_field("inherits", inherits); }

    wxString GetInheritsAsString() const;
    wxArrayString GetInheritsAsArrayNoTemplates() const;
    wxArrayString GetInheritsAsArrayWithTemplates() const;

    wxString GetTypename() const;
    wxString GetMacrodef() const;
    void SetTypename(const wxString& value);
    void SetMacrodef(const wxString& value);
    void SetTemplateDefinition(const wxString& def) { set_extra_field("template", def); }

    const wxString& GetScope() const { return m_scope; }
    void SetScope(const wxString& scope) { m_scope = scope; }

    /**
     * \return Scope name of the tag.
     * If path is empty in db or contains just the project name, it will return the literal <global>.
     * For project tags, an empty string is returned.
     */
    wxString GetScopeName() const;

    /**
     * Generate a Key for this tag based on its attributes
     * \return tag key
     */
    wxString Key() const;

    /**
     * @brief return the local variable type
     */
    wxString GetLocalType() const;

    /**
     * Generate a display name for this tag to be used by the symbol tree
     * \return tag display name
     */
    wxString GetDisplayName() const;

    /**
     * Generate a full display name for this tag that includes:
     * full scope + name + signature
     * \return tag full display name
     */
    wxString GetFullDisplayName() const;

    //------------------------------------------
    // Extenstion fields
    //------------------------------------------
    const wxString& GetExtField(const wxString& extField) const;

    //------------------------------------------
    // Misc
    //------------------------------------------
    void Print();
    TagEntryPtr ReplaceSimpleMacro();

private:
    /**
     * Update the path with full path (e.g. namespace::class)
     * \param path path to add
     */
    void UpdatePath(wxString& path);
};

#endif // CODELITE_ENTRY_H
