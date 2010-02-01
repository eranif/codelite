#ifndef TYPEDEF_H
#define TYPEDEF_H

#include <string>
#include "variable.h"

class clTypedef
{
public:
	std::string m_name;
	Variable    m_realType;
	
public:
	clTypedef()  {}
	~clTypedef() {}
	
	void print() {
		printf("Name: %s\n", m_name.c_str());
		m_realType.Print();
	}
	
	void clear() {
		m_realType.Reset();
		m_name.clear();
	}
};

typedef std::list<clTypedef> clTypedefList;
#endif // TYPEDEF_H
