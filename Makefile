OBJ=swgudpser.o ip_reader.o udp_network.o \
		mysqlwork.o
COMOPT=-I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient

app:$(OBJ)
	g++ -o app $(OBJ) $(COMOPT)

swgudpser.o:swgudpser.cpp udp_network.h ip_reader.h
	g++ -c swgudpser.cpp
ip_reader.o:ip_reader.h
udp_network.o:udp_network.h
mysqlwork.o:mysqlwork.h

.PHONY: clean
clean:
	-rm $(OBJ) app
