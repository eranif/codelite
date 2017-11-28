#include <vector>
#include <map>
#include <set>
#include <string>
#include <list>
#include <wx/arrstr.h>
#include <wx/string.h>
#include <memory>
#include <LiteEditor/frame.h>
#include <LiteEditor/manager.h>
#include <CodeLite/singleton.h>
#include <CodeLite/tag_tree.h>
#include <CodeLite/tree.h>
#include <CodeLite/tree_node.h>
#include <CodeLite/smart_ptr.h>
#include <CodeLite/entry.h>
#include <LiteEditor/context_manager.h>
#include <CodeLite/FlexLexer.h>
#include <CodeLite/lex.yy.cpp>
#include <CodeLite/y.tab.h>
#include <LiteEditor/cl_editor.h>
#include <Interfaces/debugger.h>
#include <Outline/outline.h>
#include <wx/clipbrd.h>
#include <wx/app.h>
#include <Plugin/editor_config.h>
#include <Plugin/lexer_configuration.h>
#include <wxcrafter/wx_ordered_map.h>
#include <Plugin/workspace.h>
#include <set>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <unordered_map>

struct ST {
    std::string name;
};

ST theStruct;

#define EG(x) theStruct.x

// This set of classes are here for creating entries in the database for the
// test_netsting_scope_resolving
class string
{
    class SubClass
    {
        int member;
        class SubSubClass
        {
            int sub_sub_member;
            class Box
            {
                void foo();
            };
        };
    };
};

// Needed for the test of the 'goto decl inside namespace'
namespace Baz
{
struct Tada {
    int bar;
};

class Foo
{
public:
    Tada sad; // Tada cannot be located via Goto Declaration/Implementation
};

//////////////////////////////////////////////////////////////
// Test the 'testStructDeclaratorInFuncArgument
//////////////////////////////////////////////////////////////

void foo_bar(struct wxString* pString, const std::string& secondArg) {

}
} // Namespace Baz

enum NormalEnum {
    kApple,
    kBanana,
    kOrange,
};

enum class ClassEnum : int {
    kRed,
    kGreen,
    kBlue
};

class ClassWithTemplateFunction
{
public:
    void* m_type;
    template <typename T>
    T* Get()
    {
        return (T*)m_type;
    }
};

class ClassWithMembers
{
    int member0;
    int member1;
    int member2;
    int member3;
    int member4;
    int member5;
    int member6;
    int member7;
    int member8;
    int member9;
public:
    typedef std::map<int, ClassWithMembers> Map_t;
    typedef std::map<int, ClassWithMembers>::const_iterator ConstIterator_t;
    
public:
    ClassWithMember() {}
    ~ClassWithMembers() {}
    void SetMember0(int member0) { this->member0 = member0; }
    void SetMember1(int member1) { this->member1 = member1; }
    void SetMember2(int member2) { this->member2 = member2; }
    void SetMember3(int member3) { this->member3 = member3; }
    void SetMember4(int member4) { this->member4 = member4; }
    void SetMember5(int member5) { this->member5 = member5; }
    void SetMember6(int member6) { this->member6 = member6; }
    void SetMember7(int member7) { this->member7 = member7; }
    void SetMember8(int member8) { this->member8 = member8; }
    void SetMember9(int member9) { this->member9 = member9; }
    int GetMember0() const { return member0; }
    int GetMember1() const { return member1; }
    int GetMember2() const { return member2; }
    int GetMember3() const { return member3; }
    int GetMember4() const { return member4; }
    int GetMember5() const { return member5; }
    int GetMember6() const { return member6; }
    int GetMember7() const { return member7; }
    int GetMember8() const { return member8; }
    int GetMember9() const { return member9; }
};
#define GET_APP static_cast<ClassWithMembers*>(ClassWithMembers::Instance())
typedef std::vector<ClassWithMembers> VectorTypedef_t;

// Template class with static member
template <class T>
class FooTemplate
{
    T* t;

public:
    static T* Get() { return t; }
};

//ContextManager::Get()->
typedef FooTemplate<ClassWithMembers> Foo_t;

