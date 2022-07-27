CXX ?= g++
CXXFLAGS ?= -O2

CXXFLAGS := ${CXXFLAGS} -Wall -Wextra -pthread
LIBS := -lSDL2 -lGL

all:
	${CXX} *.cpp -o raytracing ${LIBS}
clean:
	rm -f raytracing
