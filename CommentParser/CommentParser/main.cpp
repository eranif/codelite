#include <stdio.h>
#include <vector>

#include "comment_parser.h"

int main(int argc, char **argv)
{
	CommentParseResult comments;
	ParseComments("../test.h", comments);
	
	comments.print();
	return 0;
}
