##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=LogicalLayer
ConfigurationName      :=Release
WorkspacePath          :=/home/ilie/workspace/AsyncPubSub
ProjectPath            :=/home/ilie/workspace/AsyncPubSub/LogicalLayer
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
OutputFile             :=$(IntermediateDirectory)/lib$(ProjectName).a
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="LogicalLayer.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)../boost/inc 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/llvm-ar rcu
CXX      := /usr/bin/clang++
CC       := /usr/bin/clang
CXXFLAGS :=  -std=c++14  $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/llvm-as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/LogicalLayer_broker.cpp$(ObjectSuffix) $(IntermediateDirectory)/LogicalLayer_publisher.cpp$(ObjectSuffix) $(IntermediateDirectory)/LogicalLayer_subscriber.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(IntermediateDirectory) $(OutputFile)

$(OutputFile): $(Objects)
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(AR) $(ArchiveOutputSwitch)$(OutputFile) @$(ObjectsFileList) $(ArLibs)
	@$(MakeDirCommand) "/home/ilie/workspace/AsyncPubSub/.build-release"
	@echo rebuilt > "/home/ilie/workspace/AsyncPubSub/.build-release/LogicalLayer"

MakeIntermediateDirs:
	@test -d ./Release || $(MakeDirCommand) ./Release


./Release:
	@test -d ./Release || $(MakeDirCommand) ./Release

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/LogicalLayer_broker.cpp$(ObjectSuffix): LogicalLayer/broker.cpp $(IntermediateDirectory)/LogicalLayer_broker.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ilie/workspace/AsyncPubSub/LogicalLayer/LogicalLayer/broker.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/LogicalLayer_broker.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/LogicalLayer_broker.cpp$(DependSuffix): LogicalLayer/broker.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/LogicalLayer_broker.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/LogicalLayer_broker.cpp$(DependSuffix) -MM LogicalLayer/broker.cpp

$(IntermediateDirectory)/LogicalLayer_broker.cpp$(PreprocessSuffix): LogicalLayer/broker.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/LogicalLayer_broker.cpp$(PreprocessSuffix) LogicalLayer/broker.cpp

$(IntermediateDirectory)/LogicalLayer_publisher.cpp$(ObjectSuffix): LogicalLayer/publisher.cpp $(IntermediateDirectory)/LogicalLayer_publisher.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ilie/workspace/AsyncPubSub/LogicalLayer/LogicalLayer/publisher.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/LogicalLayer_publisher.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/LogicalLayer_publisher.cpp$(DependSuffix): LogicalLayer/publisher.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/LogicalLayer_publisher.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/LogicalLayer_publisher.cpp$(DependSuffix) -MM LogicalLayer/publisher.cpp

$(IntermediateDirectory)/LogicalLayer_publisher.cpp$(PreprocessSuffix): LogicalLayer/publisher.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/LogicalLayer_publisher.cpp$(PreprocessSuffix) LogicalLayer/publisher.cpp

$(IntermediateDirectory)/LogicalLayer_subscriber.cpp$(ObjectSuffix): LogicalLayer/subscriber.cpp $(IntermediateDirectory)/LogicalLayer_subscriber.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/ilie/workspace/AsyncPubSub/LogicalLayer/LogicalLayer/subscriber.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/LogicalLayer_subscriber.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/LogicalLayer_subscriber.cpp$(DependSuffix): LogicalLayer/subscriber.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/LogicalLayer_subscriber.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/LogicalLayer_subscriber.cpp$(DependSuffix) -MM LogicalLayer/subscriber.cpp

$(IntermediateDirectory)/LogicalLayer_subscriber.cpp$(PreprocessSuffix): LogicalLayer/subscriber.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/LogicalLayer_subscriber.cpp$(PreprocessSuffix) LogicalLayer/subscriber.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Release/


