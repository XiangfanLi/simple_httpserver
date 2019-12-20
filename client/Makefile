all: testclient

testclient: testclient.o httpclient.o
	g++ testclient.o httpclient.o -o testclient -O2 -std=c++11 -lpthread
	
httpclient.o: httpclient.cpp httpclient.h
	g++ -c httpclient.cpp -o httpclient.o -O2 -std=c++11 -lpthread
	
testclient.o: testclient.cpp httpclient.h
	g++ -c testclient.cpp -o testclient.o -O2 -std=c++11 -lpthread
	
clean:
	rm *.o testclient
