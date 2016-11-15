objects = error.o lx_sock.o sigchildwait.o signal.o str_echo.o 

tcpser : $(objects)

error.o : error.h
lx_sock.o : lx_sock.h
sigchildwait.o : lx_sock.h
signal.o : lx_sock.h
str_echo.o : lx_sock.h

.PHONY : clean, tcpser
clean:
	rm $(objects) 
