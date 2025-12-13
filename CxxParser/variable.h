#ifndef VARIABLE_H
#define VARIABLE_H

#include <list>
#include <string>

#ifndef WXDLLIMPEXP_CL

#ifdef WXMAKINGDLL_CL
#define WXDLLIMPEXP_CL __declspec(dllexport)
#elif defined(WXUSINGDLL_CL)
#define WXDLLIMPEXP_CL __declspec(dllimport)
#else // not making nor using DLL
#define WXDLLIMPEXP_CL
#endif

#endif

class WXDLLIMPEXP_CL Variable
{
public:
    std::string m_name;
    bool m_isTemplate{false};
    std::string m_templateDecl;
    bool m_isPtr{false};
    std::string m_type;      // as in 'int a;' -> type=int
    std::string m_typeScope; // as in 'std::string a;' -> typeScope = std, type=string
    std::string m_pattern;
    std::string m_completeType;
    std::string m_starAmp;
    int m_lineno{0};
    bool m_isConst{false};
    std::string m_rightSideConst;
    std::string m_defaultValue; // used mainly for function arguments with default values foo(int = 0);
    std::string m_arrayBrackets;
    bool m_isEllipsis{false};
    bool m_isBasicType{false};
    bool m_isVolatile{false};
    bool m_isAuto{false};
    bool m_enumInTypeDecl{false}; // e.g. enum MyENum e

public:
    Variable() = default;
    virtual ~Variable() = default;

    // copy ctor
    Variable(const Variable&) = default;

    // operator =
    Variable& operator=(const Variable&) = default;

    // clear the class content
    void Reset();

    // print the variable to stdout
    void Print() const;
};

using VariableList = std::list<Variable>;
#endif // VARIABLE_H
