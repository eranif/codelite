#ifndef FUNCTION_H
#define FUNCTION_H

#include "string"
#include "list"
#include "variable.h"

class clFunction
{
public:
	std::string 	m_name;
	std::string		m_scope;					//functions' scope
	std::string 	m_retrunValusConst;			// is the return value a const?
	std::string 	m_signature;
	Variable 		m_returnValue;
	int 			m_lineno;
	bool 			m_isVirtual;
	bool 			m_isPureVirtual;
	bool 			m_isConst;
	
public:
	clFunction();
	virtual ~clFunction();

	//clear the class content
	void Reset();
	
	//print the variable to stdout
	void Print();
};

typedef std::list<clFunction> FunctionList;
#endif //FUNCTION_H
