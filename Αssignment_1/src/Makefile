sniffer : Listener_Manager.o ../modules/DoublyList_Implementation.o ../modules/Worker.o ../modules/LinkedList.o 
	gcc Listener_Manager.o ../modules/DoublyList_Implementation.o ../modules/Worker.o ../modules/LinkedList.o -o sniffer

Listener_Manager.o : Listener_Manager.c ../include/DoublyList_Interface.h ../include/Worker_Interface.h
	gcc -c Listener_Manager.c
	
DoublyList_Implementation.o :../modules/DoublyList_Implementation.c ../include/DoublyList_Interface.h
	gcc -c ../modules/DoublyList_Implementation.c

LinkedList.o: ../modules/LinkedList.c ../include/LinkedList_Interface.h
	gcc -c ../modules/LinkedList.c

Worker.o: ../modules/Worker.c ../include/Worker_Interface.h ../include/LinkedList_Interface.h
	gcc -c ../modules/Worker.c

clean:
	rm sniffer Listener_Manager.o ../modules/DoublyList_Implementation.o ../modules/Worker.o ../modules/LinkedList.o
