tcpser : 
	cd lib; make 
	cd src; make 
	gcc -Wall lib/*.o src/*.o -o tcpser

.PHONY : clean
clean:
	rm tcpser  
	cd lib; make clean
	cd src; make clean  
