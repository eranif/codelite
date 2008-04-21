#ifndef VARIABLE_H
#define VARIABLE_H

#include "string"
#include "list"

class Variable
{
public:
	std::string 	m_name;
	bool 			m_isTemplate;
	std::string	m_templateDecl;
	bool			m_isPtr;
	std::string	m_type;		//as in 'int a;' -> type=int
	std::string	m_typeScope;//as in 'std::string a;' -> typeScope = std, type=string
	std::string 	m_pattern;
	std::string  	m_starAmp;
	int				m_lineno;
	bool 			m_isConst;
	
public:
	Variable();
	virtual ~Variable();

	//copy ctor
	Variable(const Variable& src);
	
	//operator = 
	Variable& operator=(const Variable& src);
	
	//clear the class content
	void Reset();
	
	//print the variable to stdout
	void Print();
};

typedef std::list<Variable> VariableList;
#endif //VARIABLE_H
