SRCDIR=$(PWD)
CC   = g++
SRC += $(SRCDIR)/src/lidar_server_publisher.cpp 
SRC += $(SRCDIR)/src/lidarScanner.cpp 
SRC += $(SRCDIR)/src/sockethandler.cpp 
SRC += $(SRCDIR)/src/lidarserver.cpp 

TARGET = $(SRCDIR)/build/lidarserver

LIBRARY += boost_system
LIBRARY += boost_date_time
LIBRARY += boost_thread
LIBRARY += pthread
LIBRARY += roscpp
LIBRARY += rosconsole
LIBRARY += rostime
LIBRARY += sqlite3
#LIBRARY += sqlite3


LIBRARYDIR += /usr/lib
LIBRARYDIR += /opt/ros/indigo/lib
LIBRARYDIR += /usr/lib/arm-linux-gnueabihf


LINK_LIBDIR += /opt/ros/indigo/lib


#NCLUDEDIR += $(SRCDIR)/src
INCLUDEDIR += /opt/ros/indigo/include
INCLUDEDIR += /usr/include
INCLUDEDIR += $(SRCDIR)/include


OPT = -std=c++11
DEBUG = -g
WARN= -Wall
PTHREAD= -pthread
LINK    = -Wl,-rpath,

INCDIR  = $(patsubst %,-I%,$(INCLUDEDIR))
LIBDIR  = $(patsubst %,-L%,$(LIBRARYDIR))
LIB    = $(patsubst %,-l%,$(LIBRARY))
LINKDIR = $(patsubst %,$(LINK)%,$(LINK_LIBDIR))

all:
	$(CC) $(OPT) $(SRC) -o $(TARGET) $(INCDIR) $(LIBDIR) $(LINKDIR) $(LIB) 

clean:
	rm -rf $(TARGET)