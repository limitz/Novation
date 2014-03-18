INCLUDES   = -Iinclude
LIBRARIES  = -Llib -lpthread -lasound

NOVATIONSRC = $(wildcard src/*.cpp)
NOVATIONOBJ = $(addprefix obj/novation/, $(notdir $(NOVATIONSRC:.cpp=.o)))

EXAMPLESRC = $(wildcard examples/*.cpp)
EXAMPLEBIN = $(addprefix bin/, $(notdir $(EXAMPLESRC:.cpp=)))

all: lib/libnovation.a

lib/libnovation.a: $(NOVATIONOBJ)
	@mkdir -p lib
	ar rcs $@ $^

obj/novation/%.o: src/%.cpp include/novation/%.h
	@mkdir -p obj
	@mkdir -p obj/novation
	g++ -c -o $@ $< $(INCLUDES)

examples: $(EXAMPLEBIN)

bin/%: examples/%.cpp lib
	@mkdir -p bin
	g++ -o $@ $< $(INCLUDES) -lnovation $(LIBRARIES)
	

clean:
	@rm -R obj
	@rm -R lib
