CXX = g++
CXXFLAGS = -Wall -pthread

TARGET = dali
SOURCES = main.cpp qqqDALI.cpp
HEADERS = qqqDALI.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) -lwiringPi

clean:
	rm -f $(TARGET)
