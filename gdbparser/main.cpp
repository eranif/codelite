#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <errno.h>
#include <string.h>
#include <memory.h>
#include <vector>
#include <map>

#include "gdb_result_parser.h"
#include "gdb_parser_incl.h"

char *loadFile(const char *fileName);
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

bool testParseLocals();
bool testTokens();
bool testChildrenParser();
void testRegisterNames();

int main(int argc, char **argv)
{
    //testTokens();
    testChildrenParser();
//  testRegisterNames();
    return 0;
}

void testRegisterNames()
{
    char *l = loadFile("../test.txt");
    if( !l ) {
        return;
    }
    std::vector<std::string> names;
    gdbParseRegisterNames(l, names);
    for(size_t i=0; i<names.size(); ++i) {
        printf("%s\n", names.at(i).c_str());
    }
    free(l);
}

bool testChildrenParser()
{
    char *l = loadFile("../test.txt");
    if( !l ) {
        return false;
    }
    GdbChildrenInfo info;
    gdbParseListChildren( l, info );

    info.print();
    free(l);
}

char *loadFile(const char *fileName)
{
    FILE *fp;
    long len;
    char *buf = NULL;

    fp = fopen(fileName, "rb");
    if (!fp) {
        printf("failed to open file '../test.txt': %s\n", strerror(errno));
        return NULL;
    }

    //read the whole file
    fseek(fp, 0, SEEK_END); 		//go to end
    len = ftell(fp); 					//get position at end (length)
    fseek(fp, 0, SEEK_SET); 		//go to beginning
    buf = (char *)malloc(len+1); 	//malloc buffer

    //read into buffer
    long bytes = fread(buf, sizeof(char), len, fp);
    printf("read: %ld\n", bytes);
    if (bytes != len) {
        fclose(fp);
        printf("failed to read from file 'test.h': %s\n", strerror(errno));
		free(buf);
        return NULL;
    }

    buf[len] = 0;	// make it null terminated string
    fclose(fp);
    return buf;
}

bool testTokens()
{
    char *l = loadFile("../test.txt");
    if (!l) {
        return false;
    }
    setGdbLexerInput(l, true, true);
    ReadTokens();
    gdb_result_lex_clean();
    free(l);
}

bool testParseLocals()
{
    char *l = loadFile("../test.txt");
    if (!l) {
        return false;
    }
    std::string strline = l;

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
