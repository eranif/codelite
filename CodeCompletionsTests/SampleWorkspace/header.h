#include <vector>
#include <map>
#include <set>
#include <string>
#include <list>
#include <multimap>
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
#include <SymbolView/symbolview.h>
#include <wx/clipbrd.h>
#include <wx/app.h>

struct ST {
	std::string name;
};

ST theStruct;

#define EG(x) theStruct.x
