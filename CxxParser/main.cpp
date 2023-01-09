//test the parser
#include "windows.h"
#include "stdio.h"
#include "errno.h"
#include "string.h"
#include "vector"
#include "string"
#include "map"
#include "code_completion_api.h"

void testScopeParser(char *buf);
void testVarParser(char *buf);
void testTypedefParser(char *buf);
void testExprParser(char *buf);
void testFuncParser(char *buf);
void testPureLexer(const char *buf);
void testIsPrimitive(char *buf);

char *loadFile(const char *fileName);
void testColorLocals()
{

}

int main()
{
    char *buf = loadFile("../Debug/test.h");
    //testPureLexer( buf );
    //print the scope name
    //testScopeParser(buf);
    testVarParser(buf);
    //testExprParser(buf);
    //testFuncParser(buf);
    //testTypedefParser(buf);
    //testIsPrimitive(buf);
    //testFuncParser(buf);
    free(buf);
}

void testPureLexer( const char* buf ) 
{
    CppLexer lexer( buf );
    while ( lexer.lex() != 0 ) {
        printf("%s | %d\n", lexer.text().c_str(), lexer.line_number());
    }
}

void testIsPrimitive(char *buf)
{
    printf("===== Testing is_primitive_type ======\n");
    printf("%d\n", is_primitive_type(buf) ? 1 : 0);
}

void testFuncParser(char *buf)
{
    printf("===== Testing function parser ======\n");
//	time_t start = GetTickCount();
    FunctionList li;
    //fflush(stdout);
    std::map<std::string, std::string> ignoreTokens;
    get_functions(buf, li, ignoreTokens);
//	time_t end = GetTickCount();
    for (FunctionList::iterator iter = li.begin(); iter != li.end(); iter++) {
        //test the var parser on the function argument list:
        clFunction f = (*iter);
        f.Print();
        //testVarParser((char*)f.m_signature.c_str());
        printf("%s\n", f.m_name.c_str());
    }

//	printf("total time: %d\n", end-start);
    printf("matches found: %d\n", li.size());
}

void testExprParser(char *buf)
{
    printf("===== Testing expression parser ======\n");
    ExpressionResult res = parse_expression(buf);
    res.Print();
}

void doo(int ii, int value, int stat)
{
}

void testScopeParser(char *buf)
{
    printf("===== Testing Scope parser ======\n");
//	time_t start = GetTickCount();
    std::vector<std::string> additionNS;
    std::map<std::string, std::string> ignoreTokens;

    ignoreTokens["wxT"] = true;
    std::string scope = get_scope_name(buf, additionNS, ignoreTokens);
//	time_t end = GetTickCount();
//	printf("total time: %d\n", end-start);
    printf("scope name=%s\n", scope.c_str());
    for (size_t i=0; i<additionNS.size(); i++) {
        printf("NS: %s\n", additionNS.at(i).c_str());
    }
    fflush(stdout);
}

void testVarParser(char *buf)
{
    printf("===== Testing Variable parser ======\n");
//	time_t start = GetTickCount();
    VariableList li;
//	fflush(stdout);

    std::map<std::string, std::string> ignoreTokens;
    get_variables(buf, li, ignoreTokens, true);
//	time_t end = GetTickCount();
    for (VariableList::iterator iter = li.begin(); iter != li.end(); iter++) {
        Variable var = *iter;
        var.Print();
    }

//	printf("total time: %d\n", end-start);
    printf("matches found: %d\n", li.size());
}

void testTypedefParser(char *buf)
{
    printf("===== Testing Typedef parser ======\n");
    clTypedefList li;


    std::map<std::string, std::string> ignoreTokens;
    get_typedefs(buf, li);
    for (clTypedefList::iterator iter = li.begin(); iter != li.end(); iter++) {
        clTypedef var = *iter;
        var.print();
    }
    printf("matches found: %d\n", li.size());
}

//-------------------------------------------------------
// Help function
//-------------------------------------------------------
char *loadFile(const char *fileName)
{
    FILE *fp;
    long len;
    char *buf = NULL;

    fp = fopen(fileName, "rb");
    if (!fp) {
        printf("failed to open file 'test.h': %s\n", strerror(errno));
        return NULL;
    }

    //read the whole file
    fseek(fp, 0, SEEK_END); 		//go to end
    len = ftell(fp); 					//get position at end (length)
    fseek(fp, 0, SEEK_SET); 		//go to beginning
    buf = (char *)malloc(len+1); 	//malloc buffer

    //read into buffer
    long bytes = fread(buf, sizeof(char), len, fp);
    //printf("read: %ld\n", bytes);
    if (bytes != len) {
        fclose(fp);
        printf("failed to read from file 'test.h': %s\n", strerror(errno));
        return NULL;
    }

    buf[len] = 0;	// make it null terminated string
    fclose(fp);
    return buf;
}
