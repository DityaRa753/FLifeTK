.PHONY: clean all clean_all
#dir names
ODIR = o
HDIR = h
SDIR = src
PATHLIB =

CXX  = $(shell fltk-config --cxx)
FLTKFLAGS = $(shell fltk-config --cxxflags )
LDFLAGS  = $(shell fltk-config  --ldflags )
LDSTATIC = $(shell fltk-config  --ldstaticflags )


# Compiler
CXXFLAGS = -std=c++98 -g -Wall -fno-exceptions

# Project name
PROJECT = gameoflife

# Libraries
PLIBS :=
LIBS :=
INCS := -I$(HDIR)

SRCS = $(shell find $(SDIR) -name '*.cpp' | sort)
DIRS = $(shell find $(SDIR) -type d | sed 's/$(SDIR)/./g' ) 
OBJS = $(patsubst $(SDIR)/%.cpp,$(ODIR)/%.o,$(SRCS))

# Targets
$(PROJECT): buildrepo $(OBJS)
	$(CXX) $(CXXFLAGS) $(FLTKFLAGS) $(OBJS) $(PLIBS) $(LIBS) \
		$(INCS) -o $@ $(LDFLAGS)

$(ODIR)/%.o: $(SDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(FLTKFLAGS) $(INCS) -c $< -o $@

clean:
	rm -rf $(PROJECT) $(ODIR)

buildrepo:
	mkdir -p $(ODIR)
