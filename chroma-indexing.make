# makefile for the MusicIdentification library

PROGRAM = ChromaIndexing

INCLUDEDIRS = \
	-I libraries/

LIBDIRS =

LIBS = -lm -lstdc++

CXXSOURCES = sources/BinarySearchTree.cpp sources/ChromaHash.cpp sources/ChromaIndex.cpp sources/Global.cpp sources/GlobalIndex.cpp sources/Index.cpp sources/Statistics.cpp main/chromaIndexingMain.cpp

CXXOBJECTS = $(CXXSOURCES:.cpp=.o)

CXXFLAGS = $(INCLUDEDIRS)

CXX = g++

LDFLAGS = $(LIBDIRS) $(LIBS)

all: $(PROGRAM)

$(PROGRAM): $(CXXOBJECTS)
	$(CXX) -g -o $@ $(CXXOBJECTS) $(LDFLAGS)

	@echo " "
	@echo "Compiled Successfully!"
	@echo " "

# Clean target: "make -f Makefile.Linux clean" to remove unwanted objects and executables.
clean:
	$(RM) -f $(CXXOBJECTS) $(PROGRAM)

# end makefile



