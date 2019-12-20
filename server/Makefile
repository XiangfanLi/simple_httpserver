all: testserver

testserver: testserver.o httpserver.o condition.o threadpool.o 
	g++ testserver.o httpserver.o condition.o threadpool.o -o testserver -std=c++11 -lpthread
	
condition.o: condition.cpp condition.h
	g++ -c condition.cpp -o condition.o -O2 -std=c++11 -lpthread
	
threadpoll.o: threadpool.cpp threadpool.h condition.h
	g++ -c threadpool.cpp -o threadpool.o -O2 -std=c++11 -lpthread
	
httpserver.o: httpserver.cpp httpserver.h threadpool.h condition.h
	g++ -c httpserver.cpp -o httpserver.o -O2 -std=c++11 -lpthread
	
testserver.o: testserver.cpp httpserver.h threadpool.h condition.h
	g++ -c testserver.cpp -o testserver.o -O2 -std=c++11 -lpthread
	
clean:
	rm *.o testserver
