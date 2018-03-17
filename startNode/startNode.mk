##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=startNode
ConfigurationName      :=Debug
WorkspacePath          :=/home/ilie/workspace/AsyncPubSub
ProjectPath            :=/home/ilie/workspace/AsyncPubSub/startNode
IntermediateDirectory  :=./Debug
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
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="startNode.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)../boost/inc 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)MeshNetwork $(LibrarySwitch)LogicalLayer $(LibrarySwitch)pthread $(LibrarySwitch)boost_system $(LibrarySwitch)boost_serialization 
ArLibs                 :=  "MeshNetwork" "LogicalLayer" "pthread" "boost_system" "boost_serialization" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)../MeshNetwork/Debug $(LibraryPathSwitch)../LogicalLayer/Debug $(LibraryPathSwitch)../boost/lib/linux/x64_debug 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/llvm-ar rcu
CXX      := /usr/bin/clang++
CC       := /usr/bin/clang
CXXFLAGS :=  -g -O0 -Wall -std=c++14  $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/llvm-as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/startNode_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/startNode_arguments.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/startNode_main.cpp$(ObjectSuffix): startNode/main.cpp $(IntermediateDirectory)/startNode_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ilie/workspace/AsyncPubSub/startNode/startNode/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/startNode_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/startNode_main.cpp$(DependSuffix): startNode/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/startNode_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/startNode_main.cpp$(DependSuffix) -MM startNode/main.cpp

$(IntermediateDirectory)/startNode_main.cpp$(PreprocessSuffix): startNode/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/startNode_main.cpp$(PreprocessSuffix) startNode/main.cpp

$(IntermediateDirectory)/startNode_arguments.cpp$(ObjectSuffix): startNode/arguments.cpp $(IntermediateDirectory)/startNode_arguments.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ilie/workspace/AsyncPubSub/startNode/startNode/arguments.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/startNode_arguments.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/startNode_arguments.cpp$(DependSuffix): startNode/arguments.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/startNode_arguments.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/startNode_arguments.cpp$(DependSuffix) -MM startNode/arguments.cpp

$(IntermediateDirectory)/startNode_arguments.cpp$(PreprocessSuffix): startNode/arguments.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/startNode_arguments.cpp$(PreprocessSuffix) startNode/arguments.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


