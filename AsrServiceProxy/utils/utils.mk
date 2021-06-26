##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=utils
ConfigurationName      :=Debug
WorkspacePath          := "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent"
ProjectPath            := "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/utils"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=wangzhao04
Date                   :=25/10/17
CodeLitePath           :="/home/wangzhao04/.codelite"
LinkerName             :=g++
SharedObjectLinkerName :=g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.o.i
DebugSwitch            :=-gstab
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
ObjectsFileList        :="utils.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)../log/include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := ar rcus
CXX      := g++
CC       := gcc
CXXFLAGS :=  -g $(Preprocessors)
CFLAGS   :=  -g $(Preprocessors)
ASFLAGS  := 
AS       := as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/src_event.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_semaphore.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_critsec.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_scopedlock.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_utils.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_aip_thread.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_json_utils.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_http.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_aip_sleeper.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_base64.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_aip_cycle_thread.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_url_parser.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_ipc.cpp$(ObjectSuffix) 



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
	@$(MakeDirCommand) "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/.build-debug"
	@echo rebuilt > "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/.build-debug/utils"

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


./Debug:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_event.cpp$(ObjectSuffix): src/event.cpp $(IntermediateDirectory)/src_event.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/utils/src/event.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_event.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_event.cpp$(DependSuffix): src/event.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_event.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_event.cpp$(DependSuffix) -MM "src/event.cpp"

$(IntermediateDirectory)/src_event.cpp$(PreprocessSuffix): src/event.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_event.cpp$(PreprocessSuffix) "src/event.cpp"

$(IntermediateDirectory)/src_semaphore.cpp$(ObjectSuffix): src/semaphore.cpp $(IntermediateDirectory)/src_semaphore.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/utils/src/semaphore.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_semaphore.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_semaphore.cpp$(DependSuffix): src/semaphore.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_semaphore.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_semaphore.cpp$(DependSuffix) -MM "src/semaphore.cpp"

$(IntermediateDirectory)/src_semaphore.cpp$(PreprocessSuffix): src/semaphore.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_semaphore.cpp$(PreprocessSuffix) "src/semaphore.cpp"

$(IntermediateDirectory)/src_critsec.cpp$(ObjectSuffix): src/critsec.cpp $(IntermediateDirectory)/src_critsec.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/utils/src/critsec.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_critsec.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_critsec.cpp$(DependSuffix): src/critsec.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_critsec.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_critsec.cpp$(DependSuffix) -MM "src/critsec.cpp"

$(IntermediateDirectory)/src_critsec.cpp$(PreprocessSuffix): src/critsec.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_critsec.cpp$(PreprocessSuffix) "src/critsec.cpp"

$(IntermediateDirectory)/src_scopedlock.cpp$(ObjectSuffix): src/scopedlock.cpp $(IntermediateDirectory)/src_scopedlock.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/utils/src/scopedlock.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_scopedlock.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_scopedlock.cpp$(DependSuffix): src/scopedlock.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_scopedlock.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_scopedlock.cpp$(DependSuffix) -MM "src/scopedlock.cpp"

$(IntermediateDirectory)/src_scopedlock.cpp$(PreprocessSuffix): src/scopedlock.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_scopedlock.cpp$(PreprocessSuffix) "src/scopedlock.cpp"

$(IntermediateDirectory)/src_utils.cpp$(ObjectSuffix): src/utils.cpp $(IntermediateDirectory)/src_utils.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/utils/src/utils.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_utils.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_utils.cpp$(DependSuffix): src/utils.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_utils.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_utils.cpp$(DependSuffix) -MM "src/utils.cpp"

$(IntermediateDirectory)/src_utils.cpp$(PreprocessSuffix): src/utils.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_utils.cpp$(PreprocessSuffix) "src/utils.cpp"

$(IntermediateDirectory)/src_aip_thread.cpp$(ObjectSuffix): src/aip_thread.cpp $(IntermediateDirectory)/src_aip_thread.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/utils/src/aip_thread.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_aip_thread.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_aip_thread.cpp$(DependSuffix): src/aip_thread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_aip_thread.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_aip_thread.cpp$(DependSuffix) -MM "src/aip_thread.cpp"

$(IntermediateDirectory)/src_aip_thread.cpp$(PreprocessSuffix): src/aip_thread.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_aip_thread.cpp$(PreprocessSuffix) "src/aip_thread.cpp"

$(IntermediateDirectory)/src_json_utils.cpp$(ObjectSuffix): src/json_utils.cpp $(IntermediateDirectory)/src_json_utils.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/utils/src/json_utils.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_json_utils.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_json_utils.cpp$(DependSuffix): src/json_utils.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_json_utils.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_json_utils.cpp$(DependSuffix) -MM "src/json_utils.cpp"

$(IntermediateDirectory)/src_json_utils.cpp$(PreprocessSuffix): src/json_utils.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_json_utils.cpp$(PreprocessSuffix) "src/json_utils.cpp"

$(IntermediateDirectory)/src_http.cpp$(ObjectSuffix): src/http.cpp $(IntermediateDirectory)/src_http.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/utils/src/http.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_http.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_http.cpp$(DependSuffix): src/http.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_http.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_http.cpp$(DependSuffix) -MM "src/http.cpp"

$(IntermediateDirectory)/src_http.cpp$(PreprocessSuffix): src/http.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_http.cpp$(PreprocessSuffix) "src/http.cpp"

$(IntermediateDirectory)/src_aip_sleeper.cpp$(ObjectSuffix): src/aip_sleeper.cpp $(IntermediateDirectory)/src_aip_sleeper.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/utils/src/aip_sleeper.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_aip_sleeper.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_aip_sleeper.cpp$(DependSuffix): src/aip_sleeper.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_aip_sleeper.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_aip_sleeper.cpp$(DependSuffix) -MM "src/aip_sleeper.cpp"

$(IntermediateDirectory)/src_aip_sleeper.cpp$(PreprocessSuffix): src/aip_sleeper.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_aip_sleeper.cpp$(PreprocessSuffix) "src/aip_sleeper.cpp"

$(IntermediateDirectory)/src_base64.cpp$(ObjectSuffix): src/base64.cpp $(IntermediateDirectory)/src_base64.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/utils/src/base64.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_base64.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_base64.cpp$(DependSuffix): src/base64.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_base64.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_base64.cpp$(DependSuffix) -MM "src/base64.cpp"

$(IntermediateDirectory)/src_base64.cpp$(PreprocessSuffix): src/base64.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_base64.cpp$(PreprocessSuffix) "src/base64.cpp"

$(IntermediateDirectory)/src_aip_cycle_thread.cpp$(ObjectSuffix): src/aip_cycle_thread.cpp $(IntermediateDirectory)/src_aip_cycle_thread.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/utils/src/aip_cycle_thread.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_aip_cycle_thread.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_aip_cycle_thread.cpp$(DependSuffix): src/aip_cycle_thread.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_aip_cycle_thread.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_aip_cycle_thread.cpp$(DependSuffix) -MM "src/aip_cycle_thread.cpp"

$(IntermediateDirectory)/src_aip_cycle_thread.cpp$(PreprocessSuffix): src/aip_cycle_thread.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_aip_cycle_thread.cpp$(PreprocessSuffix) "src/aip_cycle_thread.cpp"

$(IntermediateDirectory)/src_url_parser.cpp$(ObjectSuffix): src/url_parser.cpp $(IntermediateDirectory)/src_url_parser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/utils/src/url_parser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_url_parser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_url_parser.cpp$(DependSuffix): src/url_parser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_url_parser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_url_parser.cpp$(DependSuffix) -MM "src/url_parser.cpp"

$(IntermediateDirectory)/src_url_parser.cpp$(PreprocessSuffix): src/url_parser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_url_parser.cpp$(PreprocessSuffix) "src/url_parser.cpp"

$(IntermediateDirectory)/src_ipc.cpp$(ObjectSuffix): src/ipc.cpp $(IntermediateDirectory)/src_ipc.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/wangzhao04/baidu/aip/sfg/sfg-tracer-parent/utils/src/ipc.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_ipc.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_ipc.cpp$(DependSuffix): src/ipc.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_ipc.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_ipc.cpp$(DependSuffix) -MM "src/ipc.cpp"

$(IntermediateDirectory)/src_ipc.cpp$(PreprocessSuffix): src/ipc.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_ipc.cpp$(PreprocessSuffix) "src/ipc.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


