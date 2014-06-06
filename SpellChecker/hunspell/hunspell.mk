##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=hunspell
ConfigurationName      :=Release
WorkspacePath          := "C:\src\codelite"
ProjectPath            := "C:\src\codelite\SpellChecker\hunspell"
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=PC
Date                   :=06/06/14
CodeLitePath           :="D:\software\CodeLite"
LinkerName             :=D:\software\CodeLite-6.0\MinGW-4.8.1\bin\g++.exe 
SharedObjectLinkerName :=D:\software\CodeLite-6.0\MinGW-4.8.1\bin\g++.exe -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=./lib/libhunspell.a
Preprocessors          :=$(PreprocessorSwitch)UNICODE $(PreprocessorSwitch)HUNSPELL_STATIC 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="hunspell.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=D:\software\CodeLite-6.0\MinGW-4.8.1\bin\windres.exe 
LinkOptions            :=  -O2
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := D:\software\CodeLite-6.0\MinGW-4.8.1\bin\ar.exe rcu
CXX      := D:\software\CodeLite-6.0\MinGW-4.8.1\bin\g++.exe 
CC       := D:\software\CodeLite-6.0\MinGW-4.8.1\bin\gcc.exe 
CXXFLAGS :=   $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := D:\software\CodeLite-6.0\MinGW-4.8.1\bin\as.exe 


##
## User defined environment variables
##
CodeLiteDir:=D:\software\CodeLite-6.0
UNIT_TEST_PP_SRC_DIR:=D:\software\CodeLite-6.0\UnitTest++-1.3
Objects0=$(IntermediateDirectory)/affentry.cxx$(ObjectSuffix) $(IntermediateDirectory)/suggestmgr.cxx$(ObjectSuffix) $(IntermediateDirectory)/phonet.cxx$(ObjectSuffix) $(IntermediateDirectory)/hunzip.cxx$(ObjectSuffix) $(IntermediateDirectory)/hunspell.cxx$(ObjectSuffix) $(IntermediateDirectory)/hashmgr.cxx$(ObjectSuffix) $(IntermediateDirectory)/filemgr.cxx$(ObjectSuffix) $(IntermediateDirectory)/dictmgr.cxx$(ObjectSuffix) $(IntermediateDirectory)/csutil.cxx$(ObjectSuffix) $(IntermediateDirectory)/affixmgr.cxx$(ObjectSuffix) \
	$(IntermediateDirectory)/replist.cxx$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(IntermediateDirectory) $(OutputFile)

$(OutputFile): $(Objects)
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(AR) $(ArchiveOutputSwitch)$(OutputFile) @$(ObjectsFileList) $(ArLibs)
	@$(MakeDirCommand) "C:\src\codelite/.build-win_release_unicode"
	@echo rebuilt > "C:\src\codelite/.build-win_release_unicode/hunspell"

./Release:
	@$(MakeDirCommand) "./Release"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/affentry.cxx$(ObjectSuffix): affentry.cxx $(IntermediateDirectory)/affentry.cxx$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/src/codelite/SpellChecker/hunspell/affentry.cxx" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/affentry.cxx$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/affentry.cxx$(DependSuffix): affentry.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/affentry.cxx$(ObjectSuffix) -MF$(IntermediateDirectory)/affentry.cxx$(DependSuffix) -MM "affentry.cxx"

$(IntermediateDirectory)/affentry.cxx$(PreprocessSuffix): affentry.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/affentry.cxx$(PreprocessSuffix) "affentry.cxx"

$(IntermediateDirectory)/suggestmgr.cxx$(ObjectSuffix): suggestmgr.cxx $(IntermediateDirectory)/suggestmgr.cxx$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/src/codelite/SpellChecker/hunspell/suggestmgr.cxx" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/suggestmgr.cxx$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/suggestmgr.cxx$(DependSuffix): suggestmgr.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/suggestmgr.cxx$(ObjectSuffix) -MF$(IntermediateDirectory)/suggestmgr.cxx$(DependSuffix) -MM "suggestmgr.cxx"

$(IntermediateDirectory)/suggestmgr.cxx$(PreprocessSuffix): suggestmgr.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/suggestmgr.cxx$(PreprocessSuffix) "suggestmgr.cxx"

$(IntermediateDirectory)/phonet.cxx$(ObjectSuffix): phonet.cxx $(IntermediateDirectory)/phonet.cxx$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/src/codelite/SpellChecker/hunspell/phonet.cxx" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/phonet.cxx$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/phonet.cxx$(DependSuffix): phonet.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/phonet.cxx$(ObjectSuffix) -MF$(IntermediateDirectory)/phonet.cxx$(DependSuffix) -MM "phonet.cxx"

$(IntermediateDirectory)/phonet.cxx$(PreprocessSuffix): phonet.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/phonet.cxx$(PreprocessSuffix) "phonet.cxx"

$(IntermediateDirectory)/hunzip.cxx$(ObjectSuffix): hunzip.cxx $(IntermediateDirectory)/hunzip.cxx$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/src/codelite/SpellChecker/hunspell/hunzip.cxx" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/hunzip.cxx$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/hunzip.cxx$(DependSuffix): hunzip.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/hunzip.cxx$(ObjectSuffix) -MF$(IntermediateDirectory)/hunzip.cxx$(DependSuffix) -MM "hunzip.cxx"

$(IntermediateDirectory)/hunzip.cxx$(PreprocessSuffix): hunzip.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/hunzip.cxx$(PreprocessSuffix) "hunzip.cxx"

$(IntermediateDirectory)/hunspell.cxx$(ObjectSuffix): hunspell.cxx $(IntermediateDirectory)/hunspell.cxx$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/src/codelite/SpellChecker/hunspell/hunspell.cxx" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/hunspell.cxx$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/hunspell.cxx$(DependSuffix): hunspell.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/hunspell.cxx$(ObjectSuffix) -MF$(IntermediateDirectory)/hunspell.cxx$(DependSuffix) -MM "hunspell.cxx"

$(IntermediateDirectory)/hunspell.cxx$(PreprocessSuffix): hunspell.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/hunspell.cxx$(PreprocessSuffix) "hunspell.cxx"

$(IntermediateDirectory)/hashmgr.cxx$(ObjectSuffix): hashmgr.cxx $(IntermediateDirectory)/hashmgr.cxx$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/src/codelite/SpellChecker/hunspell/hashmgr.cxx" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/hashmgr.cxx$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/hashmgr.cxx$(DependSuffix): hashmgr.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/hashmgr.cxx$(ObjectSuffix) -MF$(IntermediateDirectory)/hashmgr.cxx$(DependSuffix) -MM "hashmgr.cxx"

$(IntermediateDirectory)/hashmgr.cxx$(PreprocessSuffix): hashmgr.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/hashmgr.cxx$(PreprocessSuffix) "hashmgr.cxx"

$(IntermediateDirectory)/filemgr.cxx$(ObjectSuffix): filemgr.cxx $(IntermediateDirectory)/filemgr.cxx$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/src/codelite/SpellChecker/hunspell/filemgr.cxx" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/filemgr.cxx$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/filemgr.cxx$(DependSuffix): filemgr.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/filemgr.cxx$(ObjectSuffix) -MF$(IntermediateDirectory)/filemgr.cxx$(DependSuffix) -MM "filemgr.cxx"

$(IntermediateDirectory)/filemgr.cxx$(PreprocessSuffix): filemgr.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/filemgr.cxx$(PreprocessSuffix) "filemgr.cxx"

$(IntermediateDirectory)/dictmgr.cxx$(ObjectSuffix): dictmgr.cxx $(IntermediateDirectory)/dictmgr.cxx$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/src/codelite/SpellChecker/hunspell/dictmgr.cxx" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/dictmgr.cxx$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/dictmgr.cxx$(DependSuffix): dictmgr.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/dictmgr.cxx$(ObjectSuffix) -MF$(IntermediateDirectory)/dictmgr.cxx$(DependSuffix) -MM "dictmgr.cxx"

$(IntermediateDirectory)/dictmgr.cxx$(PreprocessSuffix): dictmgr.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/dictmgr.cxx$(PreprocessSuffix) "dictmgr.cxx"

$(IntermediateDirectory)/csutil.cxx$(ObjectSuffix): csutil.cxx $(IntermediateDirectory)/csutil.cxx$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/src/codelite/SpellChecker/hunspell/csutil.cxx" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/csutil.cxx$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/csutil.cxx$(DependSuffix): csutil.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/csutil.cxx$(ObjectSuffix) -MF$(IntermediateDirectory)/csutil.cxx$(DependSuffix) -MM "csutil.cxx"

$(IntermediateDirectory)/csutil.cxx$(PreprocessSuffix): csutil.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/csutil.cxx$(PreprocessSuffix) "csutil.cxx"

$(IntermediateDirectory)/affixmgr.cxx$(ObjectSuffix): affixmgr.cxx $(IntermediateDirectory)/affixmgr.cxx$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/src/codelite/SpellChecker/hunspell/affixmgr.cxx" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/affixmgr.cxx$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/affixmgr.cxx$(DependSuffix): affixmgr.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/affixmgr.cxx$(ObjectSuffix) -MF$(IntermediateDirectory)/affixmgr.cxx$(DependSuffix) -MM "affixmgr.cxx"

$(IntermediateDirectory)/affixmgr.cxx$(PreprocessSuffix): affixmgr.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/affixmgr.cxx$(PreprocessSuffix) "affixmgr.cxx"

$(IntermediateDirectory)/replist.cxx$(ObjectSuffix): replist.cxx $(IntermediateDirectory)/replist.cxx$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/src/codelite/SpellChecker/hunspell/replist.cxx" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/replist.cxx$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/replist.cxx$(DependSuffix): replist.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/replist.cxx$(ObjectSuffix) -MF$(IntermediateDirectory)/replist.cxx$(DependSuffix) -MM "replist.cxx"

$(IntermediateDirectory)/replist.cxx$(PreprocessSuffix): replist.cxx
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/replist.cxx$(PreprocessSuffix) "replist.cxx"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) ./Release/*$(ObjectSuffix)
	$(RM) $(OutputFile)
	$(RM) $(OutputFile)
	$(RM) "../../.build-win_release_unicode/hunspell"


