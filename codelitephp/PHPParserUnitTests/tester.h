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

#include <wx/string.h>
#include <vector>
#include <wx/wxcrtvararg.h>

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
    std::vector<ITest*> m_tests;

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
    int m_testCount;

public:
    ITest()
        : m_testCount(0)
    {
        Tester::Instance()->AddTest(this);
    }
    virtual ~ITest() {}
    virtual bool test() = 0;
};

///////////////////////////////////////////////////////////
// Helper macros:
///////////////////////////////////////////////////////////

#define TEST_FUNC(Name)              \
    class Test_##Name : public ITest \
    {                                \
    public:                          \
        virtual bool test();         \
        virtual bool Name();         \
    };                               \
    Test_##Name theTest##Name;       \
    bool Test_##Name::test()         \
    {                                \
        printf("---->\n");           \
        return Name();               \
    }                                \
    bool Test_##Name::Name()

// Check values macros
#define CHECK_SIZE(actualSize, expcSize)                                                    \
    {                                                                                       \
        m_testCount++;                                                                      \
        if(actualSize == (int)expcSize) {                                                   \
            wxFprintf(stderr, "%-40s(%d): Successfull!\n", __FUNCTION__, (int)m_testCount); \
        } else {                                                                            \
            wxFprintf(stderr,                                                               \
                      "%-40s(%d): ERROR\n%s:%d: Expected size: %d, Actual Size:%d\n",       \
                      __FUNCTION__,                                                         \
                      (int)m_testCount,                                                     \
                      __FILE__,                                                             \
                      __LINE__,                                                             \
                      (int)expcSize,                                                        \
                      (int)actualSize);                                                     \
            return false;                                                                   \
        }                                                                                   \
    }

#define CHECK_STRING(str, expcStr)                                                             \
    {                                                                                          \
        ++m_testCount;                                                                         \
        if(strcmp(str, expcStr) == 0) {                                                        \
            wxFprintf(stderr, "%-40s(%d): Successfull!\n", __FUNCTION__, (int)m_testCount);    \
        } else {                                                                               \
            wxFprintf(stderr,                                                                  \
                      "%-40s(%d): ERROR\n%s:%d: Expected string: '%s', Actual string: '%s'\n", \
                      __FUNCTION__,                                                            \
                      (int)m_testCount,                                                        \
                      __FILE__,                                                                \
                      __LINE__,                                                                \
                      expcStr,                                                                 \
                      str);                                                                    \
            return false;                                                                      \
        }                                                                                      \
    }

#define CHECK_WXSTRING(str, expcStr)                                                           \
    {                                                                                          \
        ++m_testCount;                                                                         \
        if(str == expcStr) {                                                                   \
            wxFprintf(stderr, "%-40s(%d): Successfull!\n", __FUNCTION__, (int)m_testCount);    \
        } else {                                                                               \
            wxFprintf(stderr,                                                                  \
                      "%-40s(%d): ERROR\n%s:%d: Expected string: '%s', Actual string: '%s'\n", \
                      __FUNCTION__,                                                            \
                      (int)m_testCount,                                                        \
                      __FILE__,                                                                \
                      __LINE__,                                                                \
                      expcStr,                                                                 \
                      str);                                                                    \
            return false;                                                                      \
        }                                                                                      \
    }

#define CHECK_BOOL(cond)                                                               \
    {                                                                                  \
        ++m_testCount;                                                                 \
        if(cond) {                                                                     \
            wxFprintf(stderr, "%-40s(%d): Successfull!\n", __FUNCTION__, m_testCount); \
        } else {                                                                       \
            wxFprintf(stderr,                                                          \
                      "%-40s(%d): ERROR\n%s:%d: Condition FALSE: %s\n",                \
                      __FUNCTION__,                                                    \
                      (int)m_testCount,                                                \
                      __FILE__,                                                        \
                      __LINE__,                                                        \
                      #cond);                                                          \
            return false;                                                              \
        }                                                                              \
    }

#define CHECK_BOOL_INT(cond, actRes)                                                        \
    {                                                                                       \
        ++m_testCount;                                                                      \
        if(cond) {                                                                          \
            wxFprintf(stderr, "%-40s(%d): Successfull!\n", __FUNCTION__, (int)m_testCount); \
        } else {                                                                            \
            wxFprintf(stderr,                                                               \
                      "%-40s(%d): ERROR\n%s:%d: Condition FALSE: %s. Actual result: %d\n",  \
                      __FUNCTION__,                                                         \
                      (int)m_testCount,                                                     \
                      __FILE__,                                                             \
                      __LINE__,                                                             \
                      #cond,                                                                \
                      (int)actRes);                                                         \
            return false;                                                                   \
        }                                                                                   \
    }

#endif // TESTER_H
