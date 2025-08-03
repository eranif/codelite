#include "variable.h"
#include <iostream>

Variable::Variable()
{
    Reset();
}

Variable::Variable(const Variable &src)
{
    *this = src;
}

Variable & Variable::operator =(const Variable &src)
{
    m_type          = src.m_type;
    m_templateDecl  = src.m_templateDecl;
    m_name          = src.m_name;
    m_isTemplate    = src.m_isTemplate;
    m_isPtr         = src.m_isPtr;
    m_typeScope     = src.m_typeScope;
    m_pattern       = src.m_pattern;
    m_starAmp       = src.m_starAmp;
    m_lineno        = src.m_lineno;
    m_isConst       = src.m_isConst;
    m_defaultValue  = src.m_defaultValue;
    m_arrayBrackets = src.m_arrayBrackets;
    m_isEllipsis    = src.m_isEllipsis;
    m_isBasicType   = src.m_isBasicType;
    m_rightSideConst= src.m_rightSideConst;
    m_completeType  = src.m_completeType;
    m_isVolatile    = src.m_isVolatile;
    m_isAuto        = src.m_isAuto;
    m_enumInTypeDecl= src.m_enumInTypeDecl;
    return *this;
}

void Variable::Reset()
{
    m_type = "";
    m_templateDecl = "";
    m_name = "";
    m_isTemplate = false;
    m_isPtr = false;
    m_typeScope = "";
    m_pattern = "";
    m_starAmp = "";
    m_lineno = 0;
    m_isConst = false;
    m_defaultValue = "";
    m_arrayBrackets = "";
    m_isEllipsis = false;
    m_isBasicType = false;
    m_rightSideConst = "";
    m_completeType = "";
    m_isVolatile = false;
    m_isAuto = false;
    m_enumInTypeDecl = false;
}

void Variable::Print() const
{
    std::cout << "------------------" << "\n"
              << "m_name           :" << m_name.c_str() << "\n"
              << "m_defaultValue   :" << m_defaultValue.c_str() << "\n"
              << "m_lineno         :" << m_lineno << "\n"
              << "m_starAmp        :" << m_starAmp.c_str() << "\n"
              << "m_type           :" << m_type.c_str() << "\n"
              << "m_isConst        :" << m_isConst << "\n"
              << "m_rightSideConst :" << m_rightSideConst.c_str() << "\n"
              << "m_typeScope      :" << m_typeScope.c_str() << "\n"
              << "m_templateDecl   :" << m_templateDecl.c_str() << "\n"
              << "m_arrayBrackets  :" << m_arrayBrackets.c_str() << "\n"
              << "m_isPtr          :" << m_isPtr << "\n"
              << "m_isTemplate     :" << m_isTemplate << "\n"
              << "m_isEllips       :" << m_isEllipsis << "\n"
              << "m_isBasicType    :" << m_isBasicType << "\n"
              << "m_pattern        :" << m_pattern.c_str() << "\n"
              << "m_completeType   :" << m_completeType.c_str() << "\n"
              << "m_isVolatile     :" << m_isVolatile << "\n"
              << "m_isAuto         :" << m_isAuto << "\n"
              << "m_enumInTypeDecl :" << m_enumInTypeDecl << "\n";
}
