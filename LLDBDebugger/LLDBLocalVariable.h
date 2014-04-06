#ifndef LLDBLOCALVARIABLE_H
#define LLDBLOCALVARIABLE_H

#include <vector>
#include <wx/string.h>
#include "lldb/API/SBValue.h"
#include "lldb/API/SBValueList.h"
#include <wx/clntdata.h>
#include <wx/sharedptr.h>

class LLDBLocalVariable
{
public:
    typedef wxSharedPtr<LLDBLocalVariable> Ptr_t;
    typedef std::vector<LLDBLocalVariable::Ptr_t> Vect_t;

protected:
    wxString m_name;
    wxString m_value;
    wxString m_type;
    lldb::SBValue m_lldbValue;
    LLDBLocalVariable::Vect_t m_children;
    
private:
    void DoInitFromLLDBValue(lldb::SBValue value);
    
public:
    LLDBLocalVariable(lldb::SBValue value);
    LLDBLocalVariable() {}
    virtual ~LLDBLocalVariable();

    void SetType(const wxString& type) {
        this->m_type = type;
    }
    const wxString& GetType() const {
        return m_type;
    }
    lldb::SBValue GetLLDBValue() const {
        return m_lldbValue;
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

    const LLDBLocalVariable::Vect_t& GetChildren() const {
        return m_children;
    }

    LLDBLocalVariable::Vect_t& GetChildren() {
        return m_children;
    }

    bool HasChildren() const {
        return !m_children.empty();
    }
};

class LLDBLocalVariableClientData : public wxClientData
{
    LLDBLocalVariable::Ptr_t m_variable;
public:
    LLDBLocalVariableClientData(LLDBLocalVariable::Ptr_t variable) : m_variable(variable) {}
    LLDBLocalVariable::Ptr_t GetVariable() const {
        return m_variable;
    }
};


#endif // LLDBLOCALVARIABLE_H
