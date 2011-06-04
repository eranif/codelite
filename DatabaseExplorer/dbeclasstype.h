#ifndef DBECLASSTYPE_H
#define DBECLASSTYPE_H

class dbeClassType {	
public:
	enum classType{
		typeErdPanel,
		typeSqlCommandPanel		
		};	

	dbeClassType(classType clType);
	virtual ~dbeClassType();
	classType GetClassType(){ return m_clType; }

protected:
	classType m_clType;


};

#endif // DBECLASSTYPE_H
