CXX := g++
CXXFLAGS := -Wall -g
LDFLAGS :=
OBJFILES := SelectServer.o Question.o Quiz.o Player.o
TARGET := server

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~