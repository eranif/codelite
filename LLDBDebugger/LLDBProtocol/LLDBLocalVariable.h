#ifndef LLDBLOCALVARIABLE_H
#define LLDBLOCALVARIABLE_H

#include <vector>
#include <wx/string.h>
#include <wx/clntdata.h>
#include <wx/sharedptr.h>
#include "json_node.h"
#include <wx/treebase.h>
#ifndef __WXMSW__
#include <lldb/API/SBValue.h>
#endif

class LLDBLocalVariable
{
public:
    typedef wxSharedPtr<LLDBLocalVariable> Ptr_t;
    typedef std::vector<LLDBLocalVariable::Ptr_t> Vect_t;

protected:
    wxString m_name;
    wxString m_value;
    wxString m_summary;
    wxString m_type;
    bool     m_valueChanged;
    int      m_lldbId;
    bool     m_hasChildren;
    
private:
#ifndef __WXMSW__
    void DoInitFromLLDBValue(lldb::SBValue value);
#endif

public:
#ifndef __WXMSW__
    LLDBLocalVariable(lldb::SBValue value);
#endif

    LLDBLocalVariable() : m_valueChanged(false), m_lldbId(wxNOT_FOUND), m_hasChildren(false) {}
    virtual ~LLDBLocalVariable();

    void SetLldbId(int lldbId) {
        this->m_lldbId = lldbId;
    }

    int GetLldbId() const {
        return m_lldbId;
    }
    
    // Seriliazation API
    void FromJSON(const JSONElement& json);
    JSONElement ToJSON() const;

    void SetValueChanged(bool valueChanged) {
        this->m_valueChanged = valueChanged;
    }
    bool IsValueChanged() const {
        return m_valueChanged;
    }
    void SetSummary(const wxString& summary) {
        this->m_summary = summary;
    }
    const wxString& GetSummary() const {
        return m_summary;
    }
    void SetType(const wxString& type) {
        this->m_type = type;
    }
    const wxString& GetType() const {
        return m_type;
    }

    void SetName(const wxString& name) {
        this->m_name = name;
    }

    void SetValue(const wxString& value) {
        this->m_value = value;
    }

    const wxString& GetName() const {
        return m_name;
    }

    const wxString& GetValue() const {
        return m_value;
    }
    
    bool HasChildren() const {
        return m_hasChildren;
    }
};

class LLDBLocalVariableClientData : public wxTreeItemData
{
    LLDBLocalVariable::Ptr_t m_variable;
public:
    LLDBLocalVariableClientData(LLDBLocalVariable::Ptr_t variable) : m_variable(variable) {}
    LLDBLocalVariable::Ptr_t GetVariable() const {
        return m_variable;
    }
};


#endif // LLDBLOCALVARIABLE_H
