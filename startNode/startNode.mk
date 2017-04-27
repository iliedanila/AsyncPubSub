##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=startNode
ConfigurationName      :=Debug
WorkspacePath          :=/home/ilie/workspace/nodes
ProjectPath            :=/home/ilie/workspace/nodes/startNode
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Ilie Danila
Date                   :=27/04/17
CodeLitePath           :=/home/ilie/.codelite
LinkerName             :=clang++
SharedObjectLinkerName :=clang++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
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
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)nodeLib $(LibrarySwitch)LogicalLayer $(LibrarySwitch)pthread $(LibrarySwitch)boost_system $(LibrarySwitch)boost_serialization 
ArLibs                 :=  "nodeLib" "LogicalLayer" "pthread" "boost_system" "boost_serialization" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch)/usr/lib/x86_64-linux-gnu/ $(LibraryPathSwitch)../nodeLib/Debug $(LibraryPathSwitch)../LogicalLayer/Debug 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := clang++
CC       := clang
CXXFLAGS :=  -g -O0 -Wall -std=c++14  $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := llvm-as


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
$(IntermediateDirectory)/startNode_main.cpp$(ObjectSuffix): startNode/main.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ilie/workspace/nodes/startNode/startNode/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/startNode_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/startNode_main.cpp$(PreprocessSuffix): startNode/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/startNode_main.cpp$(PreprocessSuffix) startNode/main.cpp

$(IntermediateDirectory)/startNode_arguments.cpp$(ObjectSuffix): startNode/arguments.cpp 
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ilie/workspace/nodes/startNode/startNode/arguments.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/startNode_arguments.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/startNode_arguments.cpp$(PreprocessSuffix): startNode/arguments.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/startNode_arguments.cpp$(PreprocessSuffix) startNode/arguments.cpp

##
## Clean
##
clean:
	$(RM) -r ./Debug/


