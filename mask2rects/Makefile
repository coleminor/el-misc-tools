CXX = g++
CXXFLAGS = -Wall -O2 -g
LDFLAGS =
LIBS =-lgd

TARGET = mask2rects
SRCS = box.cpp image.cpp label.cpp log.cpp main.cpp \
       options.cpp random.cpp region.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: clean all dep

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBS) $^ -o $@

clean:
	rm -f $(OBJS) $(TARGET) make.deps

dep:
	$(CXX) -MM $(SRCS) > make.deps

-include make.deps
