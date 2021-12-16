myftpd: myftpd.o serveClient.o stream.o
	gcc myftpd.o serveClient.o stream.o -o myftpd

myftpd.o: myftpd.c serveClient.h ../stream.h
	gcc -c myftpd.c

stream.o: ../stream.c ../stream.h
	gcc -c ../stream.c

serveClient.o: serveClient.c serveClient.h
	gcc -c serveClient.c

clean: 
	rm *.o
