tcpcli : 
	cd lib; make 
	cd src; make 
	gcc -D_REENTRANT -Wall -g -lpthread lib/*.o src/*.o -o tcpcli

.PHONY : clean
clean:
	rm tcpcli; 
	cd lib; make clean
	cd src; make clean  
