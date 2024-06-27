CXX := clang++
CXXFLAGS := -std=c++17 -O2
INCLUDES := -I./External/glm -I./External -I./lib -I./include
SOURCES := $(wildcard *.cpp)
OBJECTS := $(SOURCES:.cpp=.o)
TARGET := game_engine_linux
LIBS := -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2main -lSDL2_ttf
LDFLAGS := -L./lib $(LIBS)

.PHONY: all clean
all: $(TARGET)
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
clean:
	rm -f $(OBJECTS) $(TARGET)

