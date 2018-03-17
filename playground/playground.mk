##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=playground
ConfigurationName      :=Release
WorkspacePath          :=/home/ilie/workspace/AsyncPubSub
ProjectPath            :=/home/ilie/workspace/AsyncPubSub/playground
IntermediateDirectory  :=./Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Ilie Danila
Date                   :=28/10/17
CodeLitePath           :=/home/ilie/.codelite
LinkerName             :=/usr/bin/clang++
SharedObjectLinkerName :=/usr/bin/clang++ -shared -fPIC
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
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=$(PreprocessorSwitch)NDEBUG 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="playground.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)../boost/inc 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)MeshNetwork $(LibrarySwitch)LogicalLayer $(LibrarySwitch)pthread $(LibrarySwitch)boost_system $(LibrarySwitch)boost_serialization 
ArLibs                 :=  "MeshNetwork" "LogicalLayer" "pthread" "boost_system" "boost_serialization" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)../MeshNetwork/Release $(LibraryPathSwitch)../LogicalLayer/Release $(LibraryPathSwitch)../boost/lib/linux/x64_release 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/llvm-ar rcu
CXX      := /usr/bin/clang++
CC       := /usr/bin/clang
CXXFLAGS :=  -O2 -Wall -std=c++14  $(Preprocessors)
CFLAGS   :=  -O2 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/llvm-as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/playground_main.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d "../.build-release/MeshNetwork" "../.build-release/LogicalLayer" $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

"../.build-release/MeshNetwork":
	@$(MakeDirCommand) "../.build-release"
	@echo stam > "../.build-release/MeshNetwork"


"../.build-release/LogicalLayer":
	@$(MakeDirCommand) "../.build-release"
	@echo stam > "../.build-release/LogicalLayer"




MakeIntermediateDirs:
	@test -d ./Release || $(MakeDirCommand) ./Release


$(IntermediateDirectory)/.d:
	@test -d ./Release || $(MakeDirCommand) ./Release

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/playground_main.cpp$(ObjectSuffix): playground/main.cpp $(IntermediateDirectory)/playground_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ilie/workspace/AsyncPubSub/playground/playground/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/playground_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/playground_main.cpp$(DependSuffix): playground/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/playground_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/playground_main.cpp$(DependSuffix) -MM playground/main.cpp

$(IntermediateDirectory)/playground_main.cpp$(PreprocessSuffix): playground/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/playground_main.cpp$(PreprocessSuffix) playground/main.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Release/


