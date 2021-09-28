CC := g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic
INCLUDE = -I ./include
SRCDIR := ./src
TESTDIR := ./test
OBJDIR := ./obj
BINDIR := ./bin
SRCS := $(wildcard $(SRCDIR)/*.cpp)
TESTS = $(wildcard $(TESTDIR)/*.cpp)
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(SRCS:.cpp=.o)))
TESTOBJS := $(addprefix $(OBJDIR)/, $(notdir $(TESTS:.cpp=.o)))
TARGETS = $(patsubst %.cpp,%,$(notdir $(TESTS)))

all : clean $(TARGETS)

$(TARGETS): $(OBJS)
	-mkdir -p $(BINDIR)
	$(CC) $(CXXFLAGS) $(INCLUDE) -o ./obj/$@.o -c ./test/$@.cpp
	$(CC) -o ./bin/$@ $(OBJS) ./obj/$@.o

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	-mkdir -p $(OBJDIR)
	$(CC) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

clean :
	-rm -rf $(OBJDIR) $(TARGETS)