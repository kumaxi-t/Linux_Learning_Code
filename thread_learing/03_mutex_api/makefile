CXX := g++
CXXFLAGS := -Wall -g -std=c++11
LDFLAGS := -lpthread

TARGETS := 01_pthread_base 02_ticket_unsafe 03_ticket_mutex

all: $(TARGETS)

01_pthread_base: 01_pthread_base.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

02_ticket_unsafe: 02_ticket_unsafe.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

03_ticket_mutex: 03_ticket_mutex.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGETS)