CC            = gcc
CXX           = g++
CXXFLAGS      = -I. -I./include -I./client -I./event -I./server

CUR_PATH = $(shell pwd)
SOURCES_SERVER_COMMON = $(CUR_PATH)/server/ServerDispatchManager.cpp
SOURCES_SERVER_MAIN = $(CUR_PATH)/main.cpp

SOURCES_SERVER_OBJS = $(CUR_PATH)/server/ServerDispatchManager.o $(CUR_PATH)/main.o


SOURCES_CLIENT_COMMON = $(CUR_PATH)/client/ClientObserver.cpp \
$(CUR_PATH)/event/Event.cpp  \
$(CUR_PATH)/event/EventHandle.cpp \
$(CUR_PATH)/event/EventTest.cpp \
$(CUR_PATH)/event/GlobalEvents.cpp \

SOURCES_CLIENT_COMMON_OBJS = $(patsubst %.cpp, %.o, $(SOURCES_CLIENT_COMMON))


SOURCES_CLIENT1_MAIN = $(CUR_PATH)/TestClient1.cpp
SOURCES_CLIENT1_MAIN_OBJS = $(CUR_PATH)/TestClient1.o $(SOURCES_CLIENT_COMMON_OBJS)

SOURCES_CLIENT2_MAIN = $(CUR_PATH)/TestClient2.cpp
SOURCES_CLIENT2_MAIN_OBJS = $(CUR_PATH)/TestClient2.o $(SOURCES_CLIENT_COMMON_OBJS)

SOURCES_CLIENT3_MAIN = $(CUR_PATH)/TestClient3.cpp
SOURCES_CLIENT3_MAIN_OBJS = $(CUR_PATH)/TestClient3.o $(SOURCES_CLIENT_COMMON_OBJS)

TARGET_SERVER = dispatch_server
TARGET_CLIENT1 = client1
TARGET_CLIENT2 = client2
TARGET_CLIENT3 = client3

all: $(TARGET_SERVER) $(TARGET_CLIENT1) $(TARGET_CLIENT2) $(TARGET_CLIENT3)

$(TARGET_SERVER): $(SOURCES_SERVER_OBJS)
	$(CXX) $^ $(CXXFLAGS) -o $@
$(TARGET_CLIENT1):$(SOURCES_CLIENT1_MAIN_OBJS)
	$(CXX) $^ $(CXXFLAGS) -o $@
$(TARGET_CLIENT2):$(SOURCES_CLIENT2_MAIN_OBJS)
	$(CXX) $^ $(CXXFLAGS) -o $@
$(TARGET_CLIENT3):$(SOURCES_CLIENT3_MAIN_OBJS)
	$(CXX) $^ $(CXXFLAGS) -o $@
%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
clean:
	rm $(CUR_PATH)/event/*.o $(CUR_PATH)/client/*.o $(CUR_PATH)/server/*.o $(CUR_PATH)/*.o $(TARGET_SERVER) $(TARGET_CLIENT1) $(TARGET_CLIENT2) $(TARGET_CLIENT3)


