yacc -dl  -t -v makefile_parser.y
move /Y y.tab.c makefile_parser.cpp
move /Y y.tab.h makefile_lexer.h