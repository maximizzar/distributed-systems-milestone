# distributed systems milestone
Use gcc on a Linux shell to compile the code. 
You should be able to run it under any Unix like system, but it's only tested under Linux Mint.

## Data exchange via sockets by [Maximilian](https://github.com/maximizzar)
The socket_server must be executed first. The socket_client needs the ipv4 address of the server as a cli argument to work. After startup it will perform both a latency and bandwidth measurement between client and server, and prints the information onto the client console.

## Data exchange via pipes by [Lucas](https://github.com/LGreubel)
The main has to be executed. After the startup it will perform latency and bandwidth measurements between a parent and a child process using two pipes to read and write data. To measure the latency it sends 1Byte 1000 times to the child process and sends 1Byte 1000time back to the parent. To measure the bandwidth, it sends datablocks of 1KiB in sizes between 1KiB-16MiB and measures the average bandwidth of the data-exchange.
It will print out the latency in ms, and the bandwidth in kbit/s and mbit/s.

## Data exchange via Shared-Memory by [Marvin](https://github.com/MarvinR1909)
