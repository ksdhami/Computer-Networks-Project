CXX := g++
CXXFLAGS := -g -Wall -std=c++11
LDFLAGS :=
OBJFILES := TCPClient.o 
TARGET := client

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	rm -f $(OBJFILES) $(TARGET) *~