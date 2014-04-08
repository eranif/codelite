#include "LLDBLocalVariable.h"
#include <lldb/API/SBError.h>
#include <lldb/API/SBAddress.h>
#include <lldb/API/SBDeclaration.h>

LLDBLocalVariable::LLDBLocalVariable(lldb::SBValue value)
{
    DoInitFromLLDBValue( value );
}

LLDBLocalVariable::~LLDBLocalVariable()
{
}

void LLDBLocalVariable::DoInitFromLLDBValue(lldb::SBValue value)
{
    m_lldbValue = value;
    SetName( value.GetName() );
    SetType( value.GetTypeName() );

    if ( value.GetType().IsPointerType() ) {
        switch(value.GetType().GetPointeeType().GetBasicType()) {
        case lldb::eBasicTypeChar:
        case lldb::eBasicTypeSignedChar:
        case lldb::eBasicTypeUnsignedChar:
        case lldb::eBasicTypeWChar:
        case lldb::eBasicTypeSignedWChar:
        case lldb::eBasicTypeUnsignedWChar:
        case lldb::eBasicTypeChar16:
        case lldb::eBasicTypeChar32: {
            // use GetSummary() to get human readable string
            const char* pvalue = value.GetSummary();
            SetValue( pvalue ? wxString(pvalue) : wxString("") );
            break;

        }
        default: {
            // not of a string type? print the address instead
            const char* pvalue = value.GetValue();
            SetValue( pvalue ? wxString(pvalue) : wxString("") );
            break;
        }
        }
    } else {
        // any other type, we use the GetValue() method
        const char* pvalue = value.GetValue();
        SetValue( pvalue ? wxString(pvalue) : wxString("") );

    }
    
    // if this value has children, add them recursievly
    if ( value.GetNumChildren() ) {
        for(size_t i=0; i<value.GetNumChildren(); ++i) {
            lldb::SBValue child = value.GetChildAtIndex(i);
            if ( child.IsValid() ) {
                LLDBLocalVariable::Ptr_t chld(new LLDBLocalVariable(child) );
                m_children.push_back( chld );
            }
        }
    }
}
