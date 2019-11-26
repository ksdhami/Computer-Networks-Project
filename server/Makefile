CXX := g++
CXXFLAGS := -g -Wall -std=c++11
LDFLAGS :=
OBJFILES := SelectServer.o Question.o Quiz.o Player.o
TARGET := server

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~