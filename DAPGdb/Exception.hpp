#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP
#include <string>

using namespace std;

class Exception
{
    string m_what;

public:
    Exception(const string& what);
    ~Exception();

    void SetWhat(const string& what) { this->m_what = what; }
    const string& GetWhat() const { return m_what; }
};

#endif // EXCEPTION_HPP
