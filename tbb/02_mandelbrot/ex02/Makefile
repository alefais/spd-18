CXX      = icc
CXXFLAGS = -Wall -Wextra -std=c++11
LIBS	 = -ltbb
OPTFLAGS = -O3

DEPS = mandel.h
OBJS = main.o

.PHONY: main clean cleanall

%.o: %.cpp $(DEPS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OPTFLAGS) $(LIBS) -o $@ -c $<

main: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OPTFLAGS) $(LIBS) -o $@ $^

clean:
	rm -f $(OBJS)

cleanall: clean
	rm -f *.o *~ *.ppm
