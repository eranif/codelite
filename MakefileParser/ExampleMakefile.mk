##
## Auto Generated makefile, please do not edit
##
ProjectName=CodeLite

type=ReleaseUnicode

## ReleaseUnicode
ifeq ($(type),ReleaseUnicode)
ConfigurationName=ReleaseUnicode
IntermediateDirectory=./ReleaseUnicode
OutDir=$(IntermediateDirectory)
LinkerName=g++
ArchiveTool=ar rcu
SharedObjectLinkerName=g++ -shared -fPIC
ObjectSuffix=.o
DebugSwitch=-gstabs 
IncludeSwitch=-I
LibrarySwitch=-l
OutputSwitch=-o 
LibraryPathSwitch=-L
PreprocessorSwitch=-D
SourceSwitch=-c 
CompilerName=g++
RcCompilerName=windres
OutputFile=../lib/gcc_lib/libCodeLiteu.a
Preprocessors=$(PreprocessorSwitch)__WX__ $(PreprocessorSwitch)WXUSINGDLL $(PreprocessorSwitch)YY_NEVER_INTERACTIVE=1 
CmpOptions=$(shell wx-config --cxxflags --debug=no --unicode=yes) -O2 $(Preprocessors)
RcCmpOptions=
LinkOptions= 
IncludePath=$(IncludeSwitch). $(IncludeSwitch)./include $(IncludeSwitch)../sdk/wxsqlite3/include 
RcIncludePath=
Libs=
LibPath=
endif

Objects=$(IntermediateDirectory)/calltip$(ObjectSuffix) $(IntermediateDirectory)/worker_thread$(ObjectSuffix) $(IntermediateDirectory)/tokenizer$(ObjectSuffix) $(IntermediateDirectory)/tag_tree$(ObjectSuffix) $(IntermediateDirectory)/symbol_tree$(ObjectSuffix) $(IntermediateDirectory)/readtags$(ObjectSuffix) $(IntermediateDirectory)/progress_dialog$(ObjectSuffix) $(IntermediateDirectory)/procutils$(ObjectSuffix) $(IntermediateDirectory)/parse_thread$(ObjectSuffix) $(IntermediateDirectory)/lex.yy$(ObjectSuffix) \
	$(IntermediateDirectory)/language$(ObjectSuffix) $(IntermediateDirectory)/fileutils$(ObjectSuffix) $(IntermediateDirectory)/dirtraverser$(ObjectSuffix) $(IntermediateDirectory)/ctags_manager$(ObjectSuffix) $(IntermediateDirectory)/cpp_scanner$(ObjectSuffix) $(IntermediateDirectory)/cl_process$(ObjectSuffix) $(IntermediateDirectory)/scope_parser$(ObjectSuffix) $(IntermediateDirectory)/cpp_lexer$(ObjectSuffix) $(IntermediateDirectory)/cpp_comment_creator$(ObjectSuffix) $(IntermediateDirectory)/tags_options_data$(ObjectSuffix) \
	$(IntermediateDirectory)/setters_getters_data$(ObjectSuffix) $(IntermediateDirectory)/archive$(ObjectSuffix) $(IntermediateDirectory)/winprocess$(ObjectSuffix) $(IntermediateDirectory)/cpp_expr_lexer$(ObjectSuffix) $(IntermediateDirectory)/cpp_expr_parser$(ObjectSuffix) $(IntermediateDirectory)/cpp_func_parser$(ObjectSuffix) $(IntermediateDirectory)/expression_result$(ObjectSuffix) $(IntermediateDirectory)/function$(ObjectSuffix) $(IntermediateDirectory)/var_parser$(ObjectSuffix) $(IntermediateDirectory)/variable$(ObjectSuffix) \
	$(IntermediateDirectory)/entry$(ObjectSuffix) $(IntermediateDirectory)/comment$(ObjectSuffix) $(IntermediateDirectory)/tags_database$(ObjectSuffix) $(IntermediateDirectory)/variable_entry$(ObjectSuffix) 

##
## Main Build Tragets 
##
all: $(IntermediateDirectory) $(OutputFile)

$(OutputFile):  $(Objects)
	$(ArchiveTool) $(OutputFile) $(Objects)

./ReleaseUnicode:
	@test -d ./ReleaseUnicode || mkdir ./ReleaseUnicode


PreBuild:


##
## Objects
##
$(IntermediateDirectory)/calltip$(ObjectSuffix): calltip.cpp $(IntermediateDirectory)/calltip$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)calltip.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/calltip$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/calltip$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/calltip$(ObjectSuffix) -MF$(IntermediateDirectory)/calltip$(ObjectSuffix).d -MM calltip.cpp

$(IntermediateDirectory)/worker_thread$(ObjectSuffix): worker_thread.cpp $(IntermediateDirectory)/worker_thread$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)worker_thread.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/worker_thread$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/worker_thread$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/worker_thread$(ObjectSuffix) -MF$(IntermediateDirectory)/worker_thread$(ObjectSuffix).d -MM worker_thread.cpp

$(IntermediateDirectory)/tokenizer$(ObjectSuffix): tokenizer.cpp $(IntermediateDirectory)/tokenizer$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)tokenizer.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/tokenizer$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tokenizer$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/tokenizer$(ObjectSuffix) -MF$(IntermediateDirectory)/tokenizer$(ObjectSuffix).d -MM tokenizer.cpp

$(IntermediateDirectory)/tag_tree$(ObjectSuffix): tag_tree.cpp $(IntermediateDirectory)/tag_tree$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)tag_tree.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/tag_tree$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tag_tree$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/tag_tree$(ObjectSuffix) -MF$(IntermediateDirectory)/tag_tree$(ObjectSuffix).d -MM tag_tree.cpp

$(IntermediateDirectory)/symbol_tree$(ObjectSuffix): symbol_tree.cpp $(IntermediateDirectory)/symbol_tree$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)symbol_tree.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/symbol_tree$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/symbol_tree$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/symbol_tree$(ObjectSuffix) -MF$(IntermediateDirectory)/symbol_tree$(ObjectSuffix).d -MM symbol_tree.cpp

$(IntermediateDirectory)/readtags$(ObjectSuffix): readtags.cpp $(IntermediateDirectory)/readtags$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)readtags.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/readtags$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/readtags$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/readtags$(ObjectSuffix) -MF$(IntermediateDirectory)/readtags$(ObjectSuffix).d -MM readtags.cpp

$(IntermediateDirectory)/progress_dialog$(ObjectSuffix): progress_dialog.cpp $(IntermediateDirectory)/progress_dialog$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)progress_dialog.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/progress_dialog$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/progress_dialog$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/progress_dialog$(ObjectSuffix) -MF$(IntermediateDirectory)/progress_dialog$(ObjectSuffix).d -MM progress_dialog.cpp

$(IntermediateDirectory)/procutils$(ObjectSuffix): procutils.cpp $(IntermediateDirectory)/procutils$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)procutils.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/procutils$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/procutils$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/procutils$(ObjectSuffix) -MF$(IntermediateDirectory)/procutils$(ObjectSuffix).d -MM procutils.cpp

$(IntermediateDirectory)/parse_thread$(ObjectSuffix): parse_thread.cpp $(IntermediateDirectory)/parse_thread$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)parse_thread.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/parse_thread$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/parse_thread$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/parse_thread$(ObjectSuffix) -MF$(IntermediateDirectory)/parse_thread$(ObjectSuffix).d -MM parse_thread.cpp

$(IntermediateDirectory)/lex.yy$(ObjectSuffix): lex.yy.cpp $(IntermediateDirectory)/lex.yy$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)lex.yy.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/lex.yy$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/lex.yy$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/lex.yy$(ObjectSuffix) -MF$(IntermediateDirectory)/lex.yy$(ObjectSuffix).d -MM lex.yy.cpp

$(IntermediateDirectory)/language$(ObjectSuffix): language.cpp $(IntermediateDirectory)/language$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)language.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/language$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/language$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/language$(ObjectSuffix) -MF$(IntermediateDirectory)/language$(ObjectSuffix).d -MM language.cpp

$(IntermediateDirectory)/fileutils$(ObjectSuffix): fileutils.cpp $(IntermediateDirectory)/fileutils$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)fileutils.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/fileutils$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/fileutils$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/fileutils$(ObjectSuffix) -MF$(IntermediateDirectory)/fileutils$(ObjectSuffix).d -MM fileutils.cpp

$(IntermediateDirectory)/dirtraverser$(ObjectSuffix): dirtraverser.cpp $(IntermediateDirectory)/dirtraverser$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)dirtraverser.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/dirtraverser$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dirtraverser$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/dirtraverser$(ObjectSuffix) -MF$(IntermediateDirectory)/dirtraverser$(ObjectSuffix).d -MM dirtraverser.cpp

$(IntermediateDirectory)/ctags_manager$(ObjectSuffix): ctags_manager.cpp $(IntermediateDirectory)/ctags_manager$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)ctags_manager.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/ctags_manager$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ctags_manager$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/ctags_manager$(ObjectSuffix) -MF$(IntermediateDirectory)/ctags_manager$(ObjectSuffix).d -MM ctags_manager.cpp

$(IntermediateDirectory)/cpp_scanner$(ObjectSuffix): cpp_scanner.cpp $(IntermediateDirectory)/cpp_scanner$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)cpp_scanner.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/cpp_scanner$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cpp_scanner$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/cpp_scanner$(ObjectSuffix) -MF$(IntermediateDirectory)/cpp_scanner$(ObjectSuffix).d -MM cpp_scanner.cpp

$(IntermediateDirectory)/cl_process$(ObjectSuffix): cl_process.cpp $(IntermediateDirectory)/cl_process$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)cl_process.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/cl_process$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cl_process$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/cl_process$(ObjectSuffix) -MF$(IntermediateDirectory)/cl_process$(ObjectSuffix).d -MM cl_process.cpp

$(IntermediateDirectory)/scope_parser$(ObjectSuffix): scope_parser.cpp $(IntermediateDirectory)/scope_parser$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)scope_parser.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/scope_parser$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/scope_parser$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/scope_parser$(ObjectSuffix) -MF$(IntermediateDirectory)/scope_parser$(ObjectSuffix).d -MM scope_parser.cpp

$(IntermediateDirectory)/cpp_lexer$(ObjectSuffix): cpp_lexer.cpp $(IntermediateDirectory)/cpp_lexer$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)cpp_lexer.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/cpp_lexer$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cpp_lexer$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/cpp_lexer$(ObjectSuffix) -MF$(IntermediateDirectory)/cpp_lexer$(ObjectSuffix).d -MM cpp_lexer.cpp

$(IntermediateDirectory)/cpp_comment_creator$(ObjectSuffix): cpp_comment_creator.cpp $(IntermediateDirectory)/cpp_comment_creator$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)cpp_comment_creator.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/cpp_comment_creator$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cpp_comment_creator$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/cpp_comment_creator$(ObjectSuffix) -MF$(IntermediateDirectory)/cpp_comment_creator$(ObjectSuffix).d -MM cpp_comment_creator.cpp

$(IntermediateDirectory)/tags_options_data$(ObjectSuffix): tags_options_data.cpp $(IntermediateDirectory)/tags_options_data$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)tags_options_data.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/tags_options_data$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tags_options_data$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/tags_options_data$(ObjectSuffix) -MF$(IntermediateDirectory)/tags_options_data$(ObjectSuffix).d -MM tags_options_data.cpp

$(IntermediateDirectory)/setters_getters_data$(ObjectSuffix): setters_getters_data.cpp $(IntermediateDirectory)/setters_getters_data$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)setters_getters_data.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/setters_getters_data$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/setters_getters_data$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/setters_getters_data$(ObjectSuffix) -MF$(IntermediateDirectory)/setters_getters_data$(ObjectSuffix).d -MM setters_getters_data.cpp

$(IntermediateDirectory)/archive$(ObjectSuffix): archive.cpp $(IntermediateDirectory)/archive$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)archive.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/archive$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/archive$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/archive$(ObjectSuffix) -MF$(IntermediateDirectory)/archive$(ObjectSuffix).d -MM archive.cpp

$(IntermediateDirectory)/winprocess$(ObjectSuffix): winprocess.cpp $(IntermediateDirectory)/winprocess$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)winprocess.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/winprocess$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/winprocess$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/winprocess$(ObjectSuffix) -MF$(IntermediateDirectory)/winprocess$(ObjectSuffix).d -MM winprocess.cpp

$(IntermediateDirectory)/cpp_expr_lexer$(ObjectSuffix): cpp_expr_lexer.cpp $(IntermediateDirectory)/cpp_expr_lexer$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)cpp_expr_lexer.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/cpp_expr_lexer$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cpp_expr_lexer$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/cpp_expr_lexer$(ObjectSuffix) -MF$(IntermediateDirectory)/cpp_expr_lexer$(ObjectSuffix).d -MM cpp_expr_lexer.cpp

$(IntermediateDirectory)/cpp_expr_parser$(ObjectSuffix): cpp_expr_parser.cpp $(IntermediateDirectory)/cpp_expr_parser$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)cpp_expr_parser.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/cpp_expr_parser$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cpp_expr_parser$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/cpp_expr_parser$(ObjectSuffix) -MF$(IntermediateDirectory)/cpp_expr_parser$(ObjectSuffix).d -MM cpp_expr_parser.cpp

$(IntermediateDirectory)/cpp_func_parser$(ObjectSuffix): cpp_func_parser.cpp $(IntermediateDirectory)/cpp_func_parser$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)cpp_func_parser.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/cpp_func_parser$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/cpp_func_parser$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/cpp_func_parser$(ObjectSuffix) -MF$(IntermediateDirectory)/cpp_func_parser$(ObjectSuffix).d -MM cpp_func_parser.cpp

$(IntermediateDirectory)/expression_result$(ObjectSuffix): expression_result.cpp $(IntermediateDirectory)/expression_result$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)expression_result.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/expression_result$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/expression_result$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/expression_result$(ObjectSuffix) -MF$(IntermediateDirectory)/expression_result$(ObjectSuffix).d -MM expression_result.cpp

$(IntermediateDirectory)/function$(ObjectSuffix): function.cpp $(IntermediateDirectory)/function$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)function.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/function$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/function$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/function$(ObjectSuffix) -MF$(IntermediateDirectory)/function$(ObjectSuffix).d -MM function.cpp

$(IntermediateDirectory)/var_parser$(ObjectSuffix): var_parser.cpp $(IntermediateDirectory)/var_parser$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)var_parser.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/var_parser$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/var_parser$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/var_parser$(ObjectSuffix) -MF$(IntermediateDirectory)/var_parser$(ObjectSuffix).d -MM var_parser.cpp

$(IntermediateDirectory)/variable$(ObjectSuffix): variable.cpp $(IntermediateDirectory)/variable$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)variable.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/variable$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/variable$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/variable$(ObjectSuffix) -MF$(IntermediateDirectory)/variable$(ObjectSuffix).d -MM variable.cpp

$(IntermediateDirectory)/entry$(ObjectSuffix): entry.cpp $(IntermediateDirectory)/entry$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)entry.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/entry$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/entry$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/entry$(ObjectSuffix) -MF$(IntermediateDirectory)/entry$(ObjectSuffix).d -MM entry.cpp

$(IntermediateDirectory)/comment$(ObjectSuffix): comment.cpp $(IntermediateDirectory)/comment$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)comment.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/comment$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/comment$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/comment$(ObjectSuffix) -MF$(IntermediateDirectory)/comment$(ObjectSuffix).d -MM comment.cpp

$(IntermediateDirectory)/tags_database$(ObjectSuffix): tags_database.cpp $(IntermediateDirectory)/tags_database$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)tags_database.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/tags_database$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/tags_database$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/tags_database$(ObjectSuffix) -MF$(IntermediateDirectory)/tags_database$(ObjectSuffix).d -MM tags_database.cpp

$(IntermediateDirectory)/variable_entry$(ObjectSuffix): variable_entry.cpp $(IntermediateDirectory)/variable_entry$(ObjectSuffix).d
	$(CompilerName) $(SourceSwitch)variable_entry.cpp $(CmpOptions)   $(OutputSwitch)$(IntermediateDirectory)/variable_entry$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/variable_entry$(ObjectSuffix).d:
	@$(CompilerName) $(CmpOptions) $(IncludePath) -MT$(IntermediateDirectory)/variable_entry$(ObjectSuffix) -MF$(IntermediateDirectory)/variable_entry$(ObjectSuffix).d -MM variable_entry.cpp

##
## Clean
##
clean:
	$(RM) $(IntermediateDirectory)/calltip$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/calltip$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/worker_thread$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/worker_thread$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/tokenizer$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/tokenizer$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/tag_tree$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/tag_tree$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/symbol_tree$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/symbol_tree$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/readtags$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/readtags$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/progress_dialog$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/progress_dialog$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/procutils$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/procutils$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/parse_thread$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/parse_thread$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/lex.yy$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/lex.yy$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/language$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/language$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/fileutils$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/fileutils$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/dirtraverser$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/dirtraverser$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/ctags_manager$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/ctags_manager$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/cpp_scanner$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cpp_scanner$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/cl_process$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cl_process$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/scope_parser$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/scope_parser$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/cpp_lexer$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cpp_lexer$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/cpp_comment_creator$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cpp_comment_creator$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/tags_options_data$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/tags_options_data$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/setters_getters_data$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/setters_getters_data$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/archive$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/archive$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/winprocess$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/winprocess$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/cpp_expr_lexer$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cpp_expr_lexer$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/cpp_expr_parser$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cpp_expr_parser$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/cpp_func_parser$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/cpp_func_parser$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/expression_result$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/expression_result$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/function$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/function$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/var_parser$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/var_parser$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/variable$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/variable$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/entry$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/entry$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/comment$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/comment$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/tags_database$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/tags_database$(ObjectSuffix).d
	$(RM) $(IntermediateDirectory)/variable_entry$(ObjectSuffix)
	$(RM) $(IntermediateDirectory)/variable_entry$(ObjectSuffix).d
	$(RM) $(OutputFile)

-include $(IntermediateDirectory)/*.d

