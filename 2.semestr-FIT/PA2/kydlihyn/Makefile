# Let us define some constants
CXX= g++
CXXFLAGS= -std=c++17 -lstdc++fs -Wall -pedantic
LIB= -lpng -lexiv2 -lstdc++fs
LD= g++
LDFLAGS = -std=c++17
NAME=kydlihyn
SRC=src
DOC=doc
AUTOR=kydlihyn

#---------------------------------------------
src = $(wildcard $(SRC)/*.cpp)

obj = $(src:.cpp=.o)

all: dep.list | compile doc

compile: $(NAME)

$(NAME): $(obj)
	$(LD) -o $@ $(LDFLAGS) $^ $(LIB)

run:
	./$(NAME)

doc: $(src) $(wildcard $(SRC)/*.h) $(wildcard $(SRC)/*.hpp) zadani.txt
	mkdir -p $(DOC)
	doxygen doc.cfg

%.o : %.cpp %.hpp %.h
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	rm -f $(SRC)/*.o
	rm -rf $(NAME) $(DOC)
	rm -f dep.list

dep.list: $(src)
	$(CXX) -MM $(src) > dp
	cat dp | sed -E 's/^.+\.o/src\/&/g' > dep.list
	rm -f dp

-include dep.list

.PHONY: clean run dep
