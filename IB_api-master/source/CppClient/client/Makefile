SOURCES = $(wildcard *.cpp)
OBJECTS = $(SOURCES:.cpp=.o)

CXXFLAGS := --std=c++11

all: libib_api.a

(%.o): %.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $*.o

libib_api.a: $(OBJECTS)
	$(AR) rcs $@ $^

.PHONY: clean

clean:
	rm ./*.o -f
	rm ./*.a -f
