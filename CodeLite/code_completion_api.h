#ifndef CODE_COMPLETION_API_H
#define CODE_COMPLETION_API_H

#include "cl_typedef.h"
#include "variable.h"
#include "function.h"
#include "expression_result.h"

extern WXDLLIMPEXP_CL std::string       get_scope_name(const std::string &in, std::vector<std::string > &additionlNS, const std::map<std::string, std::string> &ignoreTokens);
extern WXDLLIMPEXP_CL void              get_variables(const std::string &in, VariableList &li, const std::map<std::string, std::string> &ignoreMap, bool isUsedWithinFunc);
extern WXDLLIMPEXP_CL bool              is_primitive_type(const std::string &in);
extern WXDLLIMPEXP_CL void              get_functions(const std::string &in, FunctionList &li, const std::map<std::string, std::string> &ignoreTokens);
extern WXDLLIMPEXP_CL void              get_typedefs(const std::string &in, clTypedefList &li);
extern WXDLLIMPEXP_CL ExpressionResult &parse_expression(const std::string &in);

#endif // CODECOMPLETION_API_H
