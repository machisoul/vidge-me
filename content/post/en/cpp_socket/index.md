---
author: "Vidge Wong"
title: "Socket usage"
date: "2019-09-29"
description: "SIMPLE demo of socket usage"
tags: ["c/cpp"]
categories: ["HelloWorld"]
image: "img/cover/hello_world.png"
ShowToc: true
TocOpen: true
---

## Network Socket Usage Demo

### Server Side
```cpp
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

const int PORT = 7999;
const int BUFFER_SIZE = 2048;
const int MAX_CONNECTION = 10;

int main() {
  // create initial socket
  auto initial_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (initial_fd == -1) {
    std::cout << "create initial socket failed!" << std::endl;
    return -1;
  }

  // set network interface and port
  sockaddr_in server_addr{}, client_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // bind initial socket
  if (bind(initial_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    std::cout << "bind initial socket failed!" << std::endl;    
    close(initial_fd);
    return -1;
  }

  // listen
  if (listen(initial_fd, MAX_CONNECTION) == -1) {
    std::cout << "listen failed!" << std::endl;
    close(initial_fd);
    return -1;
  }

  // accept connect and get connection socket
  socklen_t client_len = sizeof(client_addr);

  while (true) {
    auto connect_fd = accept(initial_fd, (sockaddr *)&client_addr, &client_len);
    if (connect_fd == -1) {
      std::cout << "connect failed!" << std::endl;
      return -1;
    } else {
      std::cout << "client connected: " << inet_ntoa(client_addr.sin_addr)
                << "\n";
    }

    char buff[BUFFER_SIZE];
    int msg_counter = 0;
    while (true) {
      ++msg_counter;
      memset(buff, 0, BUFFER_SIZE);
      int bytes_recevied = recv(connect_fd, buff, BUFFER_SIZE, 0);
      if (bytes_recevied <= 0) {
        std::cout << "disconnect!" << std::endl;
        break;
      }
      std::cout << "Server side receive: " << buff << std::endl;
      std::string reply = "Server received " + std::to_string(msg_counter);
      send(connect_fd, reply.c_str(), reply.size(), 0);
    }

    close(connect_fd);
  }
  close(initial_fd);

  return 0;
}
```
### Client Side

```cpp
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

const int PORT = 7999;
const int BUFFER_SIZE = 2048;

int main() {
  // create client socket
  auto connect_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (connect_fd == -1) {
    std::cout << "create client socket failed!" << std::endl;
    return -1;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

  // connect to server
  if (connect(connect_fd, (sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    std::cout << "client connect to server failed!" << std::endl;
    return -1;
  }

  char buffer[BUFFER_SIZE];
  while (true) {
    std::string input;
    std::cout << "You: ";
    std::getline(std::cin, input);
    if (input == "exit") break;

    send(connect_fd, input.c_str(), input.size(), 0);

    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recv(connect_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
      std::cout << "Server disconnected.\n";
      break;
    }
    std::cout << buffer << "\n";
  }
  close(connect_fd);
  return 0;
}
```

## Domain Socket Usage Demo

### Server Size
```cpp
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

const char *SOCKET_PATH = "/tmp/my_socket";
const int BUFFER_SIZE = 2048;
const int MAX_CONNECTION = 10;

int main() {
  // create initial socket
  auto initial_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (initial_fd == -1) {
    std::cout << "create initial socket failed!" << std::endl;
    return -1;
  }

  // set domain socket path
  unlink(SOCKET_PATH);
  sockaddr_un server_addr{};
  server_addr.sun_family = AF_UNIX;
  std::strncpy(server_addr.sun_path, SOCKET_PATH,
               sizeof(server_addr.sun_path) - 1);

  // bind initial socket
  if (bind(initial_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    std::cout << "bind initial socket failed!" << std::endl;
    close(initial_fd);
    return -1;
  }

  // listen
  if (listen(initial_fd, MAX_CONNECTION) == -1) {
    std::cout << "listen failed!" << std::endl;
    close(initial_fd);
    return -1;
  }

  while (true) {
    auto connect_fd = accept(initial_fd, nullptr, nullptr);
    if (connect_fd == -1) {
      std::cout << "connect failed!" << std::endl;
      return -1;
    }

    char buff[BUFFER_SIZE];
    int msg_counter = 0;
    while (true) {
      ++msg_counter;
      memset(buff, 0, BUFFER_SIZE);
      int bytes_recevied = recv(connect_fd, buff, BUFFER_SIZE, 0);
      if (bytes_recevied <= 0) {
        std::cout << "disconnect!" << std::endl;
        break;
      }
      std::cout << "Server side receive: " << buff << std::endl;
      std::string reply = "Server received " + std::to_string(msg_counter);
      send(connect_fd, reply.c_str(), reply.size(), 0);
    }

    close(connect_fd);
  }
  close(initial_fd);

  return 0;
}
```

### Client Side

```cpp
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

const char *SOCKET_PATH = "/tmp/my_socket";
const int BUFFER_SIZE = 2048;

int main() {
  // create client socket
  auto connect_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (connect_fd == -1) {
    std::cout << "create client socket failed!" << std::endl;
    return -1;
  }

  sockaddr_un server_addr{};
  server_addr.sun_family = AF_UNIX;
  std::strncpy(server_addr.sun_path, SOCKET_PATH,
               sizeof(server_addr.sun_path) - 1);

  // connect to server
  if (connect(connect_fd, (sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    std::cout << "client connect to server failed!" << std::endl;
    return -1;
  }

  char buffer[BUFFER_SIZE];
  while (true) {
    std::string input;
    std::cout << "You: ";
    std::getline(std::cin, input);
    if (input == "exit") break;

    send(connect_fd, input.c_str(), input.size(), 0);

    memset(buffer, 0, BUFFER_SIZE);
    int bytes_received = recv(connect_fd, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
      std::cout << "Server disconnected.\n";
      break;
    }
    std::cout << buffer << "\n";
  }
  close(connect_fd);
  return 0;
}
```