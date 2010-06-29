#include <stdio.h>
#include <vector>

#include "comment_parser.h"

int main(int argc, char **argv)
{
	CommentParseResult comments;
//	ParseComments("C:\\Users\\eran\\src\\Development\\C++\\codelite\\CL_trunk\\sqlite3\\sqlite3.h", comments);
	ParseComments("../test.h", comments);
	
	comments.print();
	return 0;
}
