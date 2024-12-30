# poll

This C++ Poll-based Socket Handling Framework simplifies the usage of the poll system call, making it easier to manage file descriptors for I/O operations. The framework encapsulates poll functionality into the Poller class and provides higher-level abstractions through the SocketHandler class for managing socket events and the EchoTask class for handling application-specific business logic.

## Features

- **Encapsulation of poll**: Simplifies file descriptor monitoring and event handling.
- **Socket Management**: Efficiently handles socket listening, attachment, and detachment.
- **Task-based Design**: Clean separation of business logic from framework code.
- **Scalability**: Designed to manage a large number of connections using poll.

## Getting Started

1. Prerequisites

   - C++ 11 or later

2. Clone the Repository

```bash
git clone https://github.com/jiafie7/poll.git
cd poll
```

## Usage

1. Example Code

- handler.cpp

```c
#include "socket/socket_handler.h"

using namespace melon::socket;

int main()
{
  Singleton<LogSystem>::getInstance()->open("./../handler.log");

  SocketHandler* handler = Singleton<SocketHandler>::getInstance();

  handler->listen("127.0.0.1", 7777);

  handler->handle(1024, -1);

  return 0;
}
```

- client.cpp

```c
#include "socket/client_socket.h"

using namespace melon::socket;

int main()
{
  Singleton<LogSystem>::getInstance()->open("./../client.log");

  ClientSocket client("127.0.0.1", 7777);

  std::string msg = "hi, I am client!";
  client.send(msg.c_str(), msg.size());

  char buf[1024] = {0};
  client.recv(buf, sizeof(buf));

  log_debug("recv: %s", buf);

  client.close();

  return 0;
}
```

2. Build the Project

```bash
mkdir build
cd build
cmake ..
make
./handler
./client
```

3. Output

- handler

```bash
socket create success.
socket bind success.
socket start listening ...
socket poll timeout.
socket poll ok: ret = 1.
socket accept: conn_fd = 5.
socket poll ok: ret = 1.
recv: conn = 5, msg = hi, I am client!
socket poll ok: ret = 1.
socket hang up by peer: conn = 5.
```

- client

```bash
recv: hi, I am server!
```

## API Reference

### Poller Class

- `Poller()`: Constructor to initialize the poller.
- `~Poller()`: Destructor to clean up resources.
- `void create(int max_conn)`: Initializes the poller with a maximum number of connections.
- `void addFd(int fd)`: Adds a file descriptor to be monitored.
- `void delFd(int fd)`: Removes a file descriptor from monitoring.
- `int poll(int timeout)`: Waits for events on the monitored file descriptors.
- `bool isSet(int index, short events)`: Checks if specific events occurred on a descriptor.
- `int maxFd()`: Returns the highest file descriptor being monitored.
- `int getFd(int index)`: Retrieves the file descriptor at a specific index.

### SocketHandler Class

- `void listen(const std::string& ip, int port)`: Listens for incoming connections on the specified IP and port.
- `void attach(int fd)`: Adds a socket descriptor to the handler for monitoring.
- `void detach(int fd)`: Removes a socket descriptor from monitoring.
- `void handle(int max_conn, int timeout)`: Handles socket events with the specified maximum connections and timeout.

### EchoTask Class

- `EchoTask(int socket_fd)`: Creates a task for the specified socket file descriptor.
- `~EchoTask()`: Cleans up resources associated with the task.
- `bool run()`: Reads incoming data, processes it, and sends a response.
- `void destroy()`: Closes the socket and cleans up the task.

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Contributions

Contributions, bug reports, and feature requests are welcome. Feel free to fork the repository, open issues, or submit pull requests.
