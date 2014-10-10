#ifndef XDEBUGTESTER_H
#define XDEBUGTESTER_H

#include <map>
#include <wx/string.h>

class XDebugTester
{
public:
    typedef std::pair<wxString, wxString> StringPair_t;
    typedef std::map<wxString, XDebugTester::StringPair_t> ResultMap_t;

protected:
    XDebugTester::ResultMap_t m_results;

public:
    XDebugTester();
    virtual ~XDebugTester();

    /**
     * @brief run the xdebug test
     */
    bool RunTest();
    const XDebugTester::ResultMap_t& GetResults() const { return m_results; }
};

#endif // XDEBUGTESTER_H
