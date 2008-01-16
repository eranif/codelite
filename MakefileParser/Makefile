# Superspecial makefile by Sverre Rabbelier!

# Maaaagic variables
cc=g++
filesuffix=
file=variable
infix_lex=yy
infix_bison=tab
CCFLAGS=$(shell wx-config --cxxflags --debug=yes --unicode=yes)
LINKFLAGS=$(shell wx-config --libs std --debug=yes --unicode=yes)
# the rules.
$(file):				main.o $(file).$(infix_bison).o $(file).$(infix_lex).o
					$(cc) -g $(LINKFLAGS) -o $(file) main.o $(file).$(infix_lex).o $(file).$(infix_bison).o -lfl

main.o:					$(file).$(infix_bison).h main.cpp
					$(cc) -g $(CCFLAGS) -c main.cpp -o main.o

$(file).$(infix_bison).o:		$(file).$(infix_bison).c$(filesuffix)
					$(cc) -g $(CCFLAGS) -c $(file).$(infix_bison).c$(filesuffix) -o $(file).$(infix_bison).o

$(file).$(infix_bison).h:		$(file).y
					bison -v -d $(file).y -o $(file).$(infix_bison).c$(filesuffix)
$(file).$(infix_bison).c$(filesuffix):	$(file).y
					bison -v -d $(file).y -o $(file).$(infix_bison).c$(filesuffix)

$(file).$(infix_lex).o:			$(file).$(infix_lex).c $(file).$(infix_bison).h
					$(cc) -g -c $(CCFLAGS) $(file).$(infix_lex).c -o $(file).$(infix_lex).o

$(file).$(infix_lex).c:			$(file).l
					flex -o $(file).$(infix_lex).c $(file).l

install:				$(file).$(infix_lex).c $(file).$(infix_bison).h $(file).$(infix_bison).c$(filesuffix)
					cp $(file).$(infix_lex).c ../Plugin/$(file).$(infix_lex).cpp
					cp $(file).$(infix_bison).h ../Plugin/$(file).$(infix_lex).h
					cp $(file).$(infix_bison).c$(filesuffix) ../Plugin/$(file).$(infix_bison).cpp

clean:
					rm -f $(file) $(file).$(infix_lex).* $(file).$(infix_bison).* main.o variable.output
					rm -f *~
