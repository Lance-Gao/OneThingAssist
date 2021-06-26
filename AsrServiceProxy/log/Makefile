AR = ar
CC := g++
ARFLAGS = cr
LIB = liblog.a
RM = -rm -rf

SRCS := $(wildcard src/*.cpp)
NODIR_SRCS := $(notdir $(SRCS))
OBJS := $(patsubst %.cpp,%.o, $(NODIR_SRCS))

vpath %.hpp include
vpath %.cpp src

CURRENT_PATH := $(shell pwd)
INC_FLAGS := -I$(CURRENT_PATH)
INC_FLAGS += -I$(CURRENT_PATH)/include
INC_FLAGS += -I$(CURRENT_PATH)/../third_party/include
INC_FLAGS += -I$(CURRENT_PATH)/../utils/include

#LDFLAGS := lib/libalpha-det-prediction.a  // library to be linked
CXXFLAGS := -g -Wall -std=c++11 -pthread

all: $(LIB)

#/bin/sh: /: Is a directory  cause by these code
%.d : %.cpp
	@set -e;rm -f $@; /
	$(CC) -MM  $< > $@.$$$$; /
	sed 's,/($*/)/.o[ :]*,/1.o $@ : ,g' < $@.$$$$ > $@; /
	rm -f $@.$$$$

sinclude $(NODIR_SRCS:.cpp=.d)

$(LIB) : $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS) $(LDFLAGS)
	@cp liblog.a ../objs
%.o : %.cpp
	$(CC) -o $@ -c $< $(CXXFLAGS) $(INC_FLAGS)

clean:
	@$(RM) $(OBJS) $(LIB) ../objs/liblog.a

.PHONY: all clean
