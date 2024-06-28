//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : CxxPreProcessorExpression.h
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

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <cstdlib>
#include "codelite_exports.h"

class WXDLLIMPEXP_CL CxxPreProcessorExpression
{
public:
    enum eOperand {
        kNONE,
        kAND,
        kOR,
        kGreaterThan,
        kGreaterThanEqual,
        kLowerThan,
        kLowerThanEqual,
    };

protected:
    CxxPreProcessorExpression* m_next;
    bool m_isNot;
    eOperand m_operand;
    bool m_defined;
    bool m_valueSet;
    double m_valueLong;
private:
    /**
     * @brief return the value as 'bool'
     */
    bool DoIsTrue();
    
    /**
     * @brief return the value as 'long'
     */
    double DoGetLong();
    
public:
    CxxPreProcessorExpression(bool value);
    virtual ~CxxPreProcessorExpression();

    bool IsTrue();

    /**
     * @brief return true if this expression already assigned with a value
     */
    bool IsValueSet() const
    {
        return m_valueSet;
    }
    /**
     * @brief the current expression is from a 'defined' condition?
     */
    void SetDefined(bool defined)
    {
        this->m_defined = defined;
    }
    /**
     * @brief is the current expression is from a 'defined' condition?
     */
    bool IsDefined() const
    {
        return m_defined;
    }
    /**
     * @brief when encountering an expression of type
     * #if !(expression) => SetNot() marks the exclamation mark
     */
    void SetNot();

    /**
     * @brief set the next expression in the chain
     */
    CxxPreProcessorExpression* SetNext(CxxPreProcessorExpression::eOperand operand, CxxPreProcessorExpression* expr);

    /**
     * @brief set value to the current expression
     */
    void SetValue(bool value);
    /**
     * @brief set value to the current expression
     */
    void SetValue(double value);
};

#endif // EXPRESSION_H
