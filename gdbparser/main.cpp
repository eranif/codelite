#include "gdb_parser_incl.h"
#include "gdb_result_parser.h"

#include <cstdio>
#include <string>
#include <vector>

void MakeSubTree(int depth);
void MakeTree();
void ReadTokens();

static void printNode(const std::string& str, int depth = 0)
{
    for (int i=0; i<depth; i++)
        printf("    ");
    printf("%s\n", str.c_str());
}

static void GDB_STRIP_QUOATES(std::string &currentToken)
{
    size_t where = currentToken.find("\"");
    if (where != std::string::npos && where == 0) {
        currentToken.erase(0, 1);
    }

    where = currentToken.rfind("\"");
    if (where != std::string::npos && where == currentToken.length()-1) {
        currentToken.erase(where);
    }

    where = currentToken.find("\"\\\\");
    if (where != std::string::npos && where == 0) {
        currentToken.erase(0, 3);
    }

    where = currentToken.rfind("\"\\\\");
    if (where != std::string::npos && where == currentToken.length()-3) {
        currentToken.erase(where);
    }
}

#define GDB_LEX()\
    {\
        type = gdb_result_lex();\
        currentToken = gdb_result_string;\
    }

#define GDB_BREAK(ch)\
    if(type != (int)ch){\
        break;\
    }

static const std::string registerName =
    R"(^done,register-names=["eax","ecx","edx","ebx","esp","ebp","esi","edi","eip","eflags","cs","ss","ds","es","fs","gs","st0","st1","st2","st3","st4","st5","st6","st7","fctrl","fstat","ftag","fiseg","fioff","foseg","fooff","fop","xmm0","xmm1","xmm2","xmm3","xmm4","xmm5","xmm6","xmm7","mxcsr","","","","","","","","","al","cl","dl","bl","ah","ch","dh","bh","ax","cx","dx","bx","","bp","si","di","mm0","mm1","mm2","mm3","mm4","mm5","mm6","mm7"])";
static const std::string breakPointTable =
    R"xxx(^done,BreakpointTable={nr_rows="2",nr_cols="6",hdr=[{width="7",alignment="-1",col_name="number",colhdr="Num"},{width="14",alignment="-1",col_name="type",colhdr="Type"},{width="4",alignment="-1",col_name="disp",colhdr="Disp"},{width="3",alignment="-1",col_name="enabled",colhdr="Enb"},{width="18",alignment="-1",col_name="addr",colhdr="Address"},{width="40",alignment="2",col_name="what",colhdr="What"}],body=[{number="1",type="breakpoint",disp="keep",enabled="y",addr="<MULTIPLE>",times="0",original-location="__cxa_throw"},{number="1.1",enabled="y",addr="0x000000000047efe7",at="<__cxa_throw+7>",thread-groups=["i1"]},{number="1.2",enabled="y",addr="0x000000006577ed87",at="<__cxa_throw+7>",thread-groups=["i1"]},{number="2",type="breakpoint",disp="keep",enabled="y",addr="0x000000000042c9c9",func="MainApp::OnInit()",file="D:/src/TestArea/wxcHelloWorld/wxcHelloWorld/main.cpp",fullname="D:\\src\\TestArea\\wxcHelloWorld\\wxcHelloWorld\\main.cpp",line="19",thread-groups=["i1"],times="1",original-location="D:/src/TestArea/wxcHelloWorld/wxcHelloWorld/main.cpp:19"}]})xxx";
static const std::string tokenString =
    R"(^done,addr="0x007615f9",nr-bytes="256",total-bytes="256",next-row="0x007615fd",prev-row="0x007615f5",next-page="0x007616f9",prev-page="0x007614f9",memory=[{addr="0x007615f9",data=["0x44","0x3a","0x5c","0x73"],ascii="D:\\s"},{addr="0x007615fd",data=["0x72","0x63","0x5c","0x54"],ascii="rc\\T"},{addr="0x00761601",data=["0x65","0x73","0x74","0x41"],ascii="estA"},{addr="0x00761605",data=["0x72","0x65","0x61","0x5c"],ascii="rea\\"},{addr="0x00761609",data=["0x54","0x65","0x73","0x74"],ascii="Test"},{addr="0x0076160d",data=["0x4d","0x65","0x6d","0x44"],ascii="MemD"},{addr="0x00761611",data=["0x75","0x6d","0x70","0x5c"],ascii="ump\\"},{addr="0x00761615",data=["0x44","0x65","0x62","0x75"],ascii="Debu"},{addr="0x00761619",data=["0x67","0x2f","0x2e","0x2f"],ascii="g/./"},{addr="0x0076161d",data=["0x54","0x65","0x73","0x74"],ascii="Test"},{addr="0x00761621",data=["0x4d","0x65","0x6d","0x44"],ascii="MemD"},{addr="0x00761625",data=["0x75","0x6d","0x70","0x2e"],ascii="ump."},{addr="0x00761629",data=["0x65","0x78","0x65","0x00"],ascii="exe?"},{addr="0x0076162d",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761631",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761635",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761639",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x0076163d",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761641",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761645",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761649",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x0076164d",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761651",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761655",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761659",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x0076165d",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761661",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761665",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761669",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x0076166d",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761671",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761675",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761679",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x0076167d",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761681",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761685",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761689",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x0076168d",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761691",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761695",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x00761699",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x0076169d",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x007616a1",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x007616a5",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x007616a9",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x007616ad",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x007616b1",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x007616b5",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x007616b9",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x007616bd",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x007616c1",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x007616c5",data=["0xf0","0xad","0xba","0x0d"],ascii="????"},{addr="0x007616c9",data=["0xf0","0xad","0xba","0xab"],ascii="????"},{addr="0x007616cd",data=["0xab","0xab","0xab","0xab"],ascii="????"},{addr="0x007616d1",data=["0xab","0xab","0xab","0xee"],ascii="????"},{addr="0x007616d5",data=["0xfe","0xee","0xfe","0x00"],ascii="????"},{addr="0x007616d9",data=["0x00","0x00","0x00","0x00"],ascii="????"},{addr="0x007616dd",data=["0x00","0x00","0x00","0xea"],ascii="????"},{addr="0x007616e1",data=["0xcc","0x35","0x27","0xda"],ascii="?5'?"},{addr="0x007616e5",data=["0x3c","0x00","0x00","0x28"],ascii="<??("},{addr="0x007616e9",data=["0x17","0x76","0x00","0xc4"],ascii="?v??"},{addr="0x007616ed",data=["0x00","0x76","0x00","0xee"],ascii="?v??"},{addr="0x007616f1",data=["0xfe","0xee","0xfe","0xee"],ascii="????"},{addr="0x007616f5",data=["0xfe","0xee","0xfe","0xee"],ascii="????"}])";

static const std::string localString = R"(^done,locals=[{name="doc",value="{<wxObject> = {_vptr.wxObject = 0x672d3a28, static ms_classInfo = {m_className = 0x672094e6 L\"w\\000x\\000O\\000b\\000j\\000e\\000c\\000t\\000\", m_objectSize = 8, m_objectConstructor = 0, m_baseInfo1 = 0x0, m_baseInfo2 = 0x0, static sm_first = 0x674b3090, m_next = 0x674b6ee0, static sm_classTable = 0x980dd0}, m_refData = 0x0}, m_version = {<wxStringBase> = {static npos = 4294967295, m_pchData = 0x99759c L\"1\\000.\\000\\060\\000\"}, <No data fields>}, m_fileEncoding = {<wxStringBase> = {static npos = 4294967295, m_pchData = 0x9974bc L\"u\\000t\\000f\\000-\\000\\070\\000\"}, <No data fields>}, m_root = 0x9978e0, static ms_classInfo = {m_className = 0x6728c6cc L\"w\\000x\\000X\\000m\\000l\\000D\\000o\\000c\\000u\\000m\\000e\\000n\\000t\\000\", m_objectSize = 20, m_objectConstructor = 0, m_baseInfo1 = 0x674b6e60, m_baseInfo2 = 0x0, static sm_first = 0x674b3090, m_next = 0x0, static sm_classTable = 0x980dd0}}"}])";

void testRegisterNames()
{
    std::vector<std::string> names;
    gdbParseRegisterNames(registerName, names);
    for (const auto& name : names) {
        printf("%s\n", name.c_str());
    }
}

bool testChildrenParser()
{
    GdbChildrenInfo info;
    gdbParseListChildren(breakPointTable, info);

    info.print();

    return true;
}

bool testTokens()
{
    setGdbLexerInput(tokenString, true, true);
    ReadTokens();
    gdb_result_lex_clean();

    return true;
}

bool testParseLocals()
{
    std::string strline = localString;

    size_t pos = strline.find("{");
    if(pos != std::string::npos) {
        strline = strline.substr(pos);
    }
    pos = strline.rfind("}");
    if(pos != std::string::npos) {
        strline = strline.substr(0, pos);
    }

#ifdef __WXMAC__
    if(strline.find("^done,locals={") != std::string::npos)
#else
    if(strline.find("^done,locals=[") != std::string::npos)
#endif
    {
        strline = strline.substr(14);
    }

    if (strline.at(strline.length()-1) == ']') {
        strline = strline.erase(strline.length()-1);
    }

    setGdbLexerInput(strline, true);
    MakeTree();
    gdb_result_lex_clean();

    return true;
}

void ReadTokens()
{
    std::string currentToken;
    int type(0);
    GDB_LEX();
    while (type != 0) {
        printf("Token=%s | %d\n", currentToken.c_str(), type);
        GDB_LEX();
    }
}

void MakeTree()
{
    std::string displayLine;
    std::string currentToken;
    int type(0);

    //remove prefix
    GDB_LEX();
    while (type != 0) {
        //pattern is *always* name="somename",value="somevalue"
        //however, value can be a sub tree value="{....}"
        if (type != GDB_NAME) {
            GDB_LEX();
            continue;
        }

        //wait for the '='
        GDB_LEX();
        GDB_BREAK('=');

        GDB_LEX();
        if (type != GDB_STRING && type != GDB_ESCAPED_STRING) {
            break;
        }

        // remove quoates from the name value
        GDB_STRIP_QUOATES(currentToken);

        displayLine += currentToken;

        //comma
        GDB_LEX();
        GDB_BREAK(',');
        //value
        GDB_LEX();
        if (type != GDB_VALUE) {
            break;
        }
        GDB_LEX();
        GDB_BREAK('=');

        GDB_LEX();
        if (type != GDB_STRING) {
            break;
        }

        // remove the quoates from the value
        GDB_STRIP_QUOATES(currentToken);

        if (currentToken.at(0) == '{') {
            if (displayLine.empty() == false) {
                //open a new node for the tree
                printNode(displayLine);

                // since we dont want a dummy <unnamed> node, we remove the false
                // open brace
                std::string tmp(currentToken);
                tmp = tmp.substr(1);

                // also remove the last closing brace
                tmp = tmp.erase(tmp.length()-1, 1);

                // set new buffer to the
                gdb_result_push_buffer(tmp);

                MakeSubTree(1);

                // restore the previous buffer
                gdb_result_pop_buffer();
            }
        } else  {
            // simple case
            displayLine += " = ";

            // set new buffer to the
            gdb_result_push_buffer(currentToken);

            GDB_LEX();
            while (type != 0) {
                if (type == (int)'{') {
                    //open a new node for the tree
                    printNode(displayLine);

                    MakeSubTree(1);

                    displayLine.clear();
                    break;
                } else {
                    displayLine += currentToken;
                    displayLine += " ";
                }
                GDB_LEX();
            }
            // restore the previous buffer
            gdb_result_pop_buffer();

            if (displayLine.empty() == false) {
                printNode(displayLine);
                displayLine.clear();
            }
        }
        displayLine.clear();
        GDB_LEX();
    }
}

void MakeSubTree(int depth)
{
    //the pattern here should be
    //key = value, ....
    //where value can be a complex value:
    //key = {...}
    std::string displayLine;
    std::string currentToken;
    int type(0);

    GDB_LEX();
    while (type != 0) {
        switch (type) {
        case (int)'=':
            displayLine += "= ";
            break;
        case (int)'{': {
            //create the new child node
            // display line can be empty (in case of unnamed structures)
            if (displayLine.empty()) {
                displayLine = "<unnamed>";
            }

            //make a sub node
            printNode(displayLine, depth);
            MakeSubTree(depth++);
            displayLine.clear();
        }
        break;
        case (int)',':
            if (displayLine.empty() == false) {
                printNode(displayLine, depth);
                displayLine.clear();
            }
            break;
        case (int)'}':
            if (displayLine.empty() == false) {
                printNode(displayLine, depth);
                displayLine.clear();
            }
            return;
        default:
            displayLine += currentToken;
            displayLine += " ";
            break;
        }
        GDB_LEX();
    }

    if (type == 0 && !displayLine.empty()) {
        printNode(displayLine);
        displayLine = "";
    }
}

int main(int argc, char** argv)
{
    testTokens();
    testChildrenParser();
    testRegisterNames();
    testParseLocals();
    return 0;
}
