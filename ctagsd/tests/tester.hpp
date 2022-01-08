//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Copyright            : (C) 2015 Eran Ifrah
// File name            : tester.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef TESTER_H
#define TESTER_H

#include <vector>
#include <wx/filename.h>
#include <wx/string.h>
#include <wx/wxcrtvararg.h>

using namespace std;

class ITest;
/**
 * @class Tester
 * @author eran
 * @date 07/08/10
 * @file tester.h
 * @brief the tester class
 */
class Tester
{
    static Tester* ms_instance;
    vector<ITest*> m_tests;

public:
    static Tester* Instance();
    static void Release();

    void AddTest(ITest* t);
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
class ITest
{
protected:
    int m_testCount = 0;
    bool m_passed = false;
    wxString m_summary;
    wxString m_file;
    wxString m_test_name;
    int m_line = 0;

public:
    ITest()
        : m_testCount(0)
    {
        Tester::Instance()->AddTest(this);
    }
    virtual ~ITest() {}
    virtual bool test() = 0;
    const wxString& get_summary() const { return m_summary; }
    bool is_passed() const { return m_passed; }
    void set_passed(bool b) { m_passed = b; }
    void set_summary(const wxString& summary) { m_summary = summary; }
    void set_file_line(const wxString& file, int l)
    {
        m_file = wxFileName(file).GetFullPath();
        m_line = l;
    }
    void set_test_name(const wxString& name) { m_test_name = name; }
    const wxString& name() const { return m_test_name; }
    int line() const { return m_line; }
    const wxString& file() const { return m_file; }
    int get_check_counter() const { return m_testCount; }
};

///////////////////////////////////////////////////////////
// Helper macros:
///////////////////////////////////////////////////////////

#define TEST_FUNC(Name)                         \
    class Test_##Name : public ITest            \
    {                                           \
    public:                                     \
        virtual bool test();                    \
        virtual bool Name();                    \
    };                                          \
    Test_##Name theTest##Name;                  \
    bool Test_##Name::test() { return Name(); } \
    bool Test_##Name::Name()

// Check values macros

#define SET_FILE_LINE_NAME()           \
    set_file_line(__FILE__, __LINE__); \
    set_test_name(__FUNCTION__)

#define CHECK_SIZE(actualSize, expcSize)                                                                 \
    {                                                                                                    \
        m_testCount++;                                                                                   \
        SET_FILE_LINE_NAME();                                                                            \
        set_passed(actualSize == (int)expcSize);                                                         \
        if(!is_passed()) {                                                                               \
            set_summary(wxString() << "Expected size: " << expcSize << ". Actual size: " << actualSize); \
            return false;                                                                                \
        }                                                                                                \
    }

#define CHECK_STRING(str, expcStr)                                                                             \
    {                                                                                                          \
        ++m_testCount;                                                                                         \
        SET_FILE_LINE_NAME();                                                                                  \
        set_passed(strcmp(str, expcStr) == 0);                                                                 \
        if(!is_passed()) {                                                                                     \
            set_summary(wxString() << "Expected string: '" << expcStr << "'. Actual string: '" << str << "'"); \
            return false;                                                                                      \
        }                                                                                                      \
    }

#define CHECK_STRING_ONE_OF(str, expected1, expected2)                                            \
    {                                                                                             \
        ++m_testCount;                                                                            \
        SET_FILE_LINE_NAME();                                                                     \
        set_passed(strcmp(str, expected1) == 0 || strcmp(str, expected2) == 0);                   \
        if(!is_passed()) {                                                                        \
            set_summary(wxString() << "Expected string on of: [" << expected1 << "," << expected2 \
                                   << "]. Actual string: '" << str << "'");                       \
            return false;                                                                         \
        }                                                                                         \
    }

#define CHECK_WXSTRING(str, expcStr)                                                                           \
    {                                                                                                          \
        ++m_testCount;                                                                                         \
        SET_FILE_LINE_NAME();                                                                                  \
        set_passed(str == expcStr);                                                                            \
        if(!is_passed()) {                                                                                     \
            set_summary(wxString() << "Expected string: '" << expcStr << "'. Actual string: '" << str << "'"); \
            return false;                                                                                      \
        }                                                                                                      \
    }

#define CHECK_BOOL(cond)                                                      \
    {                                                                         \
        ++m_testCount;                                                        \
        SET_FILE_LINE_NAME();                                                 \
        set_passed((cond));                                                   \
        if(!is_passed()) {                                                    \
            set_summary(wxString() << "Condition failed. `" << #cond << "`"); \
            return false;                                                     \
        }                                                                     \
    }

#define CHECK_NOT_NULL(ptr)                                 \
    {                                                       \
        ++m_testCount;                                      \
        SET_FILE_LINE_NAME();                               \
        set_passed((ptr));                                  \
        if(!is_passed()) {                                  \
            set_summary(wxString() << #ptr << " is null!"); \
            return false;                                   \
        }                                                   \
    }

#define CHECK_EXPECTED(expr, expected)                                                            \
    {                                                                                             \
        ++m_testCount;                                                                            \
        SET_FILE_LINE_NAME();                                                                     \
        set_passed((expr) == (expected));                                                         \
        if(!is_passed()) {                                                                        \
            set_summary(wxString() << "CHECK_EXPECTED failed. " << #expr << " != " << #expected); \
            return false;                                                                         \
        }                                                                                         \
    }

#endif // TESTER_H
