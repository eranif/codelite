#include "tester.h"
#include <stdio.h>

Tester* Tester::ms_instance = 0;

Tester::Tester()
{
}

Tester::~Tester()
{
}

Tester* Tester::Instance()
{
    if(ms_instance == 0) {
        ms_instance = new Tester();
    }
    return ms_instance;
}

void Tester::Release()
{
    if(ms_instance) {
        delete ms_instance;
    }
    ms_instance = 0;
}

void Tester::AddTest(ITest *t)
{
    m_tests.push_back( t );
}

void Tester::RunTests()
{
    size_t totalTests = m_tests.size();
    size_t success    = 0;
    size_t errors     = 0;
    for(size_t i=0; i<m_tests.size(); i++) {
        m_tests[i]->test() ? success++ : errors++;
    }


    printf("\n====> Summary: <====\n\n");

    if(success == totalTests) {
        printf("    All tests passed successfully!!\n");
    } else {
        printf("    %u of %u tests passed\n", (int)success, (int)totalTests);
        printf("    %u of %u tests failed\n", (int)errors,  (int)totalTests);
    }
}
