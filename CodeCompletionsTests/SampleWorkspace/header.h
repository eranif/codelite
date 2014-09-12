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
struct Tada
{
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

void foo_bar(struct wxString *s) {
    
}
