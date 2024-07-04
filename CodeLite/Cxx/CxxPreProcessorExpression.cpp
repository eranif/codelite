#include "CxxPreProcessorExpression.h"
#include <wx/window.h>
#include <wx/utils.h>

CxxPreProcessorExpression::CxxPreProcessorExpression(bool value)
    : m_next(NULL)
    , m_isNot(false)
    , m_operand(kNONE)
    , m_defined(false)
    , m_valueSet(false)
    , m_valueLong(value ? 1 : 0)
{
}

CxxPreProcessorExpression::~CxxPreProcessorExpression()
{
    wxDELETE(m_next);
}

bool CxxPreProcessorExpression::IsTrue()
{
    if(m_next && m_operand == kAND) {
        return DoIsTrue() && m_next->IsTrue();

    } else if(m_next && m_operand == kOR) {
        return DoIsTrue() || m_next->IsTrue();

    } else if(m_next && m_operand == kGreaterThan) {
        return DoGetLong() > m_next->DoGetLong();

    } else if(m_next && m_operand == kGreaterThanEqual) {
        return DoGetLong() >= m_next->DoGetLong();
        
    } else if(m_next && m_operand == kLowerThan) {
        return DoGetLong() < m_next->DoGetLong();

    } else if(m_next && m_operand == kLowerThanEqual) {
        return DoGetLong() <= m_next->DoGetLong();

    } else {
        return DoIsTrue();
    }
}

bool CxxPreProcessorExpression::DoIsTrue()
{
    if(m_isNot) {
        return !m_valueLong;
    } else {
        return m_valueLong;
    }
}

CxxPreProcessorExpression* CxxPreProcessorExpression::SetNext(CxxPreProcessorExpression::eOperand operand,
                                                        CxxPreProcessorExpression* expr)
{
    m_next = expr;
    m_operand = operand;
    return m_next;
}

void CxxPreProcessorExpression::SetNot()
{
    m_isNot = !m_isNot;
}

void CxxPreProcessorExpression::SetValue(bool value)
{
    m_valueLong = value ? 1 : 0;
    m_valueSet = true;
}

double CxxPreProcessorExpression::DoGetLong()
{
    return m_valueLong;
}

void CxxPreProcessorExpression::SetValue(double value)
{
    m_valueLong = value;
    m_valueSet = true;
}
