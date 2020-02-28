VARS_OLD := $(.VARIABLES)

DEPSDIR = .deps
OBJDIR = .obj
SRCDIR = src
TESTDIR = test

DEPFLAGS=-MT $@ -MMD -MP -MF $(DEPSDIR)/$*.d
FLAGS=-I/usr/include/antlr4-runtime/ -g -std=c++11
LIBS=-lantlr4-runtime

PARSER = PhilippeParser PhilippeLexer
SRC = main $(PARSER)
OBJPATH = $(patsubst %, $(OBJDIR)/%.o, $(SRC))

MAIN = main

PARSERDIR = $(SRCDIR)/parser
PARSERH = $(patsubst %, $(PARSERDIR)/%.h, $(PARSER))
PARSERSRC = $(patsubst %, $(PARSERDIR)/%.cpp, $(PARSER))

GRAMMARFILE = Philippe.g4

all: $(MAIN) test

cleancompile:
	rm -f $(MAIN)
	rm -rf $(OBJDIR)
	rm -rf $(DEPSDIR)

cleanparser:
	rm -rf $(PARSERDIR)

cleantest:
	rm -rf $(TESTDIR)

clean: cleancompile cleanparser cleantest

.PHONY: clean cleancompile cleanparser cleantest

$(PARSERH) $(PARSERSRC): $(GRAMMARFILE) | $(PARSERDIR)
	antlr4 -Dlanguage=Cpp $(GRAMMARFILE) -o src/parser -visitor

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(DEPSDIR)/%.d $(PARSERH) | $(DEPSDIR) $(OBJDIR)
	g++ $(DEPFLAGS) -c -o $@ $< $(FLAGS)

$(OBJDIR)/%.o: $(PARSERDIR)/%.cpp $(PARSERH)
	g++ -c -o $@ $< $(FLAGS) -w

$(DEPSDIR): ; mkdir -p $@
$(OBJDIR): ; mkdir -p $@
$(PARSERDIR): ; mkdir -p $@

DEPS := main
DEPSFILES := $(patsubst %,$(DEPSDIR)/%.d, $(DEPS))
$(DEPSFILES):
include $(wildcard $(DEPSFILES))

$(MAIN): $(OBJPATH)
	g++ -o $@ $^ $(FLAGS) $(LIBS)

$(TESTDIR)/PhilippeParser.java: $(GRAMMARFILE)
	mkdir -p $(TESTDIR)
	antlr4 $(GRAMMARFILE) -o $(TESTDIR)

$(TESTDIR)/PhilippeParser.class: $(TESTDIR)/PhilippeParser.java
	javac $(TESTDIR)/*.java

test: $(TESTDIR)/PhilippeParser.class

vars:; $(foreach v, $(filter-out $(VARS_OLD) VARS_OLD,$(.VARIABLES)), $(info $(v) = $($(v)))) @#noop




