# Simple Makefile

CXX = g++
CXXFLAGS = -Wall -O2 -IUtils -ISensors -IPubSub -IFusion -IArbiter
TARGET = main.exe

# Add all .cpp files to compile:
SRC	 = main.cpp \
       Utils/Utils.cpp \
       Sensors/Radar.cpp \
       Sensors/Lidar.cpp \
	   Sensors/Sensor.cpp \
	   Pubsub/Broker.cpp \
	   Arbiter/Arbiter.cpp \
	   Arbiter/LidarArbiter.cpp \
	   Arbiter/RadarArbiter.cpp \



.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run:./$(TARGET)

clean:
	rm -f $(TARGET)