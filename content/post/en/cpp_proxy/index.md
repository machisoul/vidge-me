---
author: "Vidge Wong"
title: "Proxy Pattern"
date: "2021-06-20"
description: "Understanding Proxy Pattern with Proxy-Skeleton implementation in C++"
tags: ["c/cpp"]
categories: ["Design Pattern"]
image: "img/cover/design.png"
ShowToc: true
TocOpen: true
---

## Introduction

The Proxy pattern provides a surrogate or placeholder for another object to control access to it. In distributed systems, this pattern is often implemented using a Proxy-Skeleton architecture, where:

- **Proxy**: Acts as a local representative for an object in a different address space
- **Skeleton**: Sits on the server side and forwards requests to the actual object

## Basic Proxy-Skeleton Architecture

```cpp
#include <iostream>
#include <string>
#include <memory>

// Common interface
class IService {
public:
    virtual void process(const std::string& data) = 0;
    virtual std::string getData() = 0;
    virtual ~IService() = default;
};

// Real service implementation
class RealService : public IService {
private:
    std::string data;

public:
    void process(const std::string& input) override {
        std::cout << "RealService processing: " << input << std::endl;
        data = "Processed: " + input;
    }

    std::string getData() override {
        return data;
    }
};

// Skeleton (Server side)
class ServiceSkeleton : public IService {
private:
    std::unique_ptr<RealService> realService;

public:
    ServiceSkeleton() : realService(std::make_unique<RealService>()) {}

    void process(const std::string& data) override {
        std::cout << "Skeleton received request" << std::endl;
        realService->process(data);
    }

    std::string getData() override {
        std::cout << "Skeleton forwarding getData request" << std::endl;
        return realService->getData();
    }
};

// Proxy (Client side)
class ServiceProxy : public IService {
private:
    // In real implementation, this would be a network connection
    ServiceSkeleton* remoteSkeleton;

    void logCall(const std::string& method) {
        std::cout << "Proxy: Calling " << method << std::endl;
    }

public:
    ServiceProxy(ServiceSkeleton* skeleton) : remoteSkeleton(skeleton) {}

    void process(const std::string& data) override {
        logCall("process");
        // In real implementation, this would serialize the call
        remoteSkeleton->process(data);
    }

    std::string getData() override {
        logCall("getData");
        // In real implementation, this would serialize the call
        return remoteSkeleton->getData();
    }
};
```

## Remote Service Example

Here's a more realistic example showing remote service communication:

```cpp
// Message structure for network communication
struct Message {
    enum Type { PROCESS, GET_DATA } type;
    std::string data;
};

// Network simulation
class NetworkChannel {
public:
    static void sendMessage(const Message& msg) {
        std::cout << "Network: Sending message type " 
                  << (msg.type == Message::PROCESS ? "PROCESS" : "GET_DATA")
                  << std::endl;
    }

    static Message receiveMessage() {
        std::cout << "Network: Receiving response" << std::endl;
        return Message{Message::GET_DATA, "response data"};
    }
};

// Enhanced Proxy with network communication
class RemoteServiceProxy : public IService {
private:
    void sendRequest(const Message& msg) {
        NetworkChannel::sendMessage(msg);
    }

public:
    void process(const std::string& data) override {
        Message msg{Message::PROCESS, data};
        sendRequest(msg);
    }

    std::string getData() override {
        Message msg{Message::GET_DATA, ""};
        sendRequest(msg);
        Message response = NetworkChannel::receiveMessage();
        return response.data;
    }
};

// Enhanced Skeleton with request handling
class RemoteServiceSkeleton {
private:
    RealService service;

public:
    void handleRequest(const Message& msg) {
        switch (msg.type) {
            case Message::PROCESS:
                service.process(msg.data);
                break;
            case Message::GET_DATA:
                service.getData();
                break;
        }
    }
};
```

## Usage Example

```cpp
int main() {
    // Local proxy-skeleton example
    ServiceSkeleton skeleton;
    ServiceProxy proxy(&skeleton);

    std::cout << "\n=== Local Proxy-Skeleton Test ===\n";
    proxy.process("test data");
    std::cout << "Result: " << proxy.getData() << std::endl;

    // Remote proxy example
    std::cout << "\n=== Remote Proxy Test ===\n";
    RemoteServiceProxy remoteProxy;
    remoteProxy.process("remote test");
    std::cout << "Remote result: " << remoteProxy.getData() << std::endl;

    return 0;
}
```

Output:
```
=== Local Proxy-Skeleton Test ===
Proxy: Calling process
Skeleton received request
RealService processing: test data
Proxy: Calling getData
Skeleton forwarding getData request
Result: Processed: test data

=== Remote Proxy Test ===
Network: Sending message type PROCESS
Network: Sending message type GET_DATA
Network: Receiving response
Remote result: response data
```

## Key Components

1. **IService**: Common interface for both proxy and real service
2. **RealService**: Actual service implementation
3. **ServiceSkeleton**: Server-side component that forwards requests
4. **ServiceProxy**: Client-side component that represents the remote service
5. **NetworkChannel**: Simulates network communication (in real implementation, this would use actual networking)

## Best Practices

1. **Interface Design**:
   - Keep interfaces simple and cohesive
   - Use clear method names that reflect remote nature

2. **Error Handling**:
   - Add proper error handling for network failures
   - Consider timeout mechanisms

3. **Performance**:
   - Implement caching where appropriate
   - Consider batch operations for multiple calls

4. **Security**:
   - Add authentication/authorization checks
   - Validate input data

## Conclusion

The Proxy-Skeleton pattern is particularly useful in distributed systems where objects need to communicate across different address spaces. It provides a clean separation between interface and implementation while handling the complexities of remote communication. 