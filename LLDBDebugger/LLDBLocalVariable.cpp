#include "LLDBLocalVariable.h"
#include <lldb/API/SBError.h>
#include <lldb/API/SBAddress.h>

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
    const char* pvalue = value.GetValue();
    SetValue( pvalue ? wxString(pvalue) : wxString("") );

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
