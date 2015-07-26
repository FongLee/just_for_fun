TCP Server and Client
---

###client_test
This is a simple programe as a client.
Usage:

```shell

#etc: client_test -p 192.168.1.109 -c 500 -t 5 
./client_test [-p address] [-c number of connection] [-t number of child progress]

```

###server1

When new connection comes, it fork child progress to handle it.


###server2
The parent progress creates progress poll to handle new connection.The connection is accept in the child progress.

Usage:

```shell

#etc: ./server2 -t 15 
./server2  [-t number of child progress]

```

###server3
The parent progress creates progress poll to handle new connection.The connection is accept(+lock) in the child progress.

Usage:

```shell

#etc: ./server3 -t 15 
./server3  [-t number of child progress]

```

###server4
The parent progress acccepts new connection, then copy file descripter of connection to child process pool, which handles the connection. 

Usage:

```shell

#etc: ./server4 -t 15 
./server4  [-t number of child progress]

```

###server5

When new connection comes, main thread creates  child thread to handle it.

###server6
Main thread create child thread pool to handle connection from client.The connection is accept(+lock) in the child thread.

Usage:

```shell

#etc: ./server6 -t 15 
./server6  [-t number of child thread]

```

###server7
Main thread accept new connection, and child thread handle the connection.

Usage:

```shell

#etc: ./server7 -t 15 
./server7  [-t number of child thread]

```

###client
It is a  P2P client with select.

###p2pclient

It is a  P2P client with a parent progress and a child progress.

###p2pserver
Parent process get data from client, child process send data from stdin to client.When parent process detects that client is closed, it 
closes itself and send siganl to child process to let child process close.