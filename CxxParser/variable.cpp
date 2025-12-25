#include "variable.h"

#include <iostream>

void Variable::Reset()
{
    *this = Variable();
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
              << "m_isEllipsis     :" << m_isEllipsis << "\n"
              << "m_isBasicType    :" << m_isBasicType << "\n"
              << "m_pattern        :" << m_pattern.c_str() << "\n"
              << "m_completeType   :" << m_completeType.c_str() << "\n"
              << "m_isVolatile     :" << m_isVolatile << "\n"
              << "m_isAuto         :" << m_isAuto << "\n"
              << "m_enumInTypeDecl :" << m_enumInTypeDecl << "\n";
}
