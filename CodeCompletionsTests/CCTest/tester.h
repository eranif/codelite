#ifndef TESTER_H
#define TESTER_H

#include <vector>

class ITest;
/**
 * @class Tester
 * @author eran
 * @date 07/08/10
 * @file tester.h
 * @brief the tester class
 */
class Tester {

	static Tester*         ms_instance;
	std::vector< ITest* >  m_tests;
	
public:
	static Tester* Instance();
	static void Release();
	
	void AddTest( ITest *t );
	void RunTests();
	
private:
	Tester();
	~Tester();
};


/**
 * @class ITest
 * @author eran
 * @date 07/08/10
 * @file tester.h
 * @brief the test interface
 */
class ITest {
public:
	ITest() {
		Tester::Instance()->AddTest(this);
	}
	virtual ~ITest(){}
	virtual bool test() = 0;
};

///////////////////////////////////////////////////////////
// Helper macros:
///////////////////////////////////////////////////////////

#define TEST_FUNC(Name)                     \
class Test##Name : public ITest {           \
public:                                     \
    virtual bool test();                    \
    virtual bool Name();                    \
};                                          \
Test##Name theTest##Name;                   \
bool Test##Name::test() {                   \
    return Name();                          \
}                                           \
bool Test##Name::Name()


// Check values macros
#define CHECK_SIZE(actualSize, expcSize) { if(actualSize == expcSize) {\
			printf("%s: Successfull!\n", __FUNCTION__);\
			return true;\
		} else {\
			printf("%s: ERROR: Expected size: %d, Actual Size:%d\n", __FUNCTION__, expcSize, actualSize);\
			return false;\
		}\
	}

#define CHECK_STRING(str, expcStr) { if(strcmp(str, expcStr) == 0) {\
			printf("%s: Successfull!\n", __FUNCTION__);\
			return true;\
		} else {\
			printf("%s: ERROR: Expected string: %s, Actual string:%s\n", __FUNCTION__, expcStr, str);\
			return false;\
		}\
	}

#endif // TESTER_H
