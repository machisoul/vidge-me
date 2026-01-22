---
author: "Vidge Wong"
title: "Method Communication Based on DDS Pub/Sub"
date: "2025-08-22"
description: "A lightweight RPC framework implementation using DDS Pub/Sub infrastructure for request-response communication patterns"
tags: ["vehicle", "communication", "c/cpp"]
categories: ["AutoSAR"]
image: img/cover/automotive.jpeg
hiddenInList: true
ShowToc: true
TocOpen: true
---

## Overview

This document introduces a lightweight Method (RPC) communication framework built on top of DDS (Data Distribution Service) Pub/Sub infrastructure. It provides a comparison analysis with AUTOSAR ara::com and gRPC.

### Core Features

- **Based on Pub/Sub**: Leverages existing DDS Subscribe/Publish interfaces without additional transport layer
- **Request-Response Semantics**: Implements RPC abstraction on top of Pub/Sub
- **Multi-Instance Support**: Supports multiple instances of the same Method via instance_id
- **Automatic Routing**: Uses Protobuf message types for automatic routing without manual topic specification
- **Protocol Buffers**: MethodHeader (RPC metadata) + User payload
- **Multiple Call Modes**: Synchronous, Asynchronous (Future), Fire-and-Forget
- **Thread Pool Support**: Server-side multi-threaded concurrent request processing
- **Out-of-Order Responses**: Supports request out-of-order response via correlation_id
- **Lightweight**: Built on existing infrastructure with no additional dependencies

---

## Architecture Design

### Communication Model

```
┌─────────────────────────────────────────────────────────────────┐
│                      Method Communication                        │
│                   (Pub/Sub-based RPC Pattern)                    │
└─────────────────────────────────────────────────────────────────┘

Client (MethodProxy)                    Server (MethodSkeleton)
┌──────────────────┐                   ┌──────────────────┐
│  ClientModule    │                   │  ServerModule    │
│                  │                   │                  │
│  proxy_->Init()  │                   │  skeleton_->     │
│                  │                   │    Start(4)      │
│                  │                   │                  │
│  Subscribe<      │                   │  Subscribe<      │
│   ResponseT>     │                   │   RequestT>      │
│       ↓          │                   │       ↓          │
│  OnResponseRcvd  │                   │  OnRequestRcvd   │
│       ↓          │                   │       ↓          │
│  Match by        │                   │  ThreadPool      │
│  correlation_id  │                   │  Process()       │
│       ↓          │                   │       ↓          │
│  cv_.notify()    │                   │  Publish<        │
│                  │                   │   ResponseT>     │
│                  │                   │                  │
│  Request(req,    │  ──────────>      │  Filter by       │
│   resp, 5000ms)  │   Publish<        │  instance_id     │
│                  │   RequestT>       │                  │
│  Wait on cv_     │  <──────────      │  Send Response   │
│                  │   Publish<        │                  │
│                  │   ResponseT>      │                  │
└──────────────────┘                   └──────────────────┘
         │                                      │
         └──────────────────┬───────────────────┘
                            │
         ┌──────────────────────────────────────┐
         │      DDS Pub/Sub Infrastructure       │
         │  - Type-based routing (RequestT/      │
         │    ResponseT automatically route)     │
         │  - Subscribe<T>(callback, bufsize)    │
         │  - Publish<T>(msg) -> Result          │
         │  - Auto-discovery (no topic strings)  │
         └──────────────────────────────────────┘
```

### Key Design Decisions

1. **Type Routing vs Topic Routing**
   - Does not use `/request/method_name` or `/response/method_name` topics
   - DDS Subscribe/Publish templates automatically route based on Protobuf message types
   - Example: `Subscribe<AddRequestProto>()` automatically subscribes to all AddRequestProto messages

2. **MethodHeader Design**
   - **MethodHeader**: Unified RPC metadata container
     - RPC metadata: correlation_id, instance_id, status_code, etc.
     - Can be extended with DDS-specific metadata (timestamp, domain, seq_number) via implementation
   - Advantages: Single entry point, avoids field duplication, clearer hierarchy

3. **Instance Filtering Mechanism**
   - Server filters requests by `instance_id`
   - Client automatically filters responses by `correlation_id` uniqueness (no additional client_id field needed)
   - Supports multiple Method instances of the same Protobuf type

---

## Protocol Buffers Message Design

### MethodHeader Definition

```protobuf
// proto/method_header.proto
syntax = "proto2";
package method;

message MethodHeader {
  // === Method Instance Identification ===
  // Note: Method prototype is defined by proto types (RequestT, ResponseT)
  // instance_id is only needed when multiple instances of same proto exist
  optional string instance_id = 1;        // Empty for single instance, identifier for multiple instances

  // === Request-Response Correlation ===
  // Format: "{client_uuid}_{sequence}" - globally unique across all clients
  // Used by client to match responses in pending_ map
  // Server echoes this back unchanged for response correlation
  optional string correlation_id = 2;

  // === Control Fields ===
  optional bool fire_and_forget = 3;      // true = no response expected
  optional int32 status_code = 4;         // Response status (0 = success, negative = error)

  // === Optional DDS Metadata (implementation-specific) ===
  optional int64 timestamp_us = 5;        // Timestamp in microseconds
  optional uint32 seq_number = 6;         // Sequence number
}
```

### User Message Definition Example

```protobuf
// proto/methods/add_method.proto
syntax = "proto2";
package method;

import "proto/method_header.proto";

message AddRequestProto {
    optional MethodHeader method_header = 1;  // RPC metadata
    optional int32 a = 2;                     // User payload
    optional int32 b = 3;
}

message AddResponseProto {
    optional MethodHeader method_header = 1;  // RPC metadata
    optional int32 result = 2;
}
```

**Field Description**:
- `method_header`: Contains all RPC metadata (correlation_id for matching, status_code for error codes, etc.)
- User fields: Start from field 2, concise and clear

---

## API Design

### Error Code Definition

```cpp
// method/method_common.h
enum class MethodError : int32_t {
  // Success
  kSuccess = 0,

  // Common errors
  kTimeout = -1,
  kNotInitialized = -2,
  kPublishFailed = -3,
  kAlreadyStarted = -4,
  kAlreadyInitialized = -5,
  kNotStarted = -6,

  // User-defined error codes start from -100
  kUserErrorBase = -100
};

// Helper functions
inline int32_t ToInt(MethodError error);
inline MethodError ToMethodError(int32_t code);
```

### MethodSkeleton API (Server)

```cpp
template <typename RequestT, typename ResponseT>
class MethodSkeleton {
 public:
  // Constructor
  // @param instance_id: Instance identifier (empty for single instance)
  explicit MethodSkeleton(const std::string& instance_id = "");

  virtual ~MethodSkeleton();

  // Start the Method service
  // @param dds_participant: DDS participant for Subscribe/Publish
  // @param thread_num: Number of worker threads (1 = single-threaded)
  // @return: 0 (kSuccess) on success, negative error code on failure
  int32_t Start(DDSParticipant* dds_participant, uint32_t thread_num = 1);

  // Stop the Method service
  // @return: 0 (kSuccess) on success, negative error code on failure
  int32_t Stop();

  // Process function (pure virtual) - user must implement
  // @param req: Request message
  // @param resp: Response message to be filled
  // @return: 0 (kSuccess) on success, or user-defined error code (< kUserErrorBase)
  virtual int32_t Process(const std::shared_ptr<RequestT>& req,
                          std::shared_ptr<ResponseT>& resp) = 0;

 private:
  void OnRequestReceived(const std::shared_ptr<const RequestT>& req);
  void ProcessAndRespond(const std::shared_ptr<const RequestT>& req);

  DDSParticipant* participant_;
  std::string instance_id_;
  std::unique_ptr<ThreadPool> thread_pool_;
  bool started_;
  std::mutex mutex_;
};
```

### MethodProxy API (Client)

```cpp
template <typename RequestT, typename ResponseT>
class MethodProxy {
 public:
  // Constructor
  // @param instance_id: Instance identifier (empty for single instance)
  explicit MethodProxy(const std::string& instance_id = "");

  virtual ~MethodProxy();

  // Initialize the proxy
  // @param participant: DDS participant instance
  // @return: 0 (kSuccess) on success, negative error code on failure
  int32_t Init(DDSParticipant* participant);

  // Check if method service is online
  // @return: true if service is available
  bool IsMethodOnline();

  // Synchronous request
  // @param req: Request message
  // @param resp: Response message to be filled
  // @param timeout_ms: Timeout in milliseconds
  // @return: 0 (kSuccess) on success, negative error code on failure or timeout,
  //          or server-returned error code
  int32_t Request(std::shared_ptr<RequestT> req,
                  std::shared_ptr<ResponseT> resp, int64_t timeout_ms);

  // Asynchronous request
  // @param req: Request message
  // @param timeout_ms: Timeout in milliseconds
  // @return: Future containing status code and response
  std::future<std::pair<int32_t, std::shared_ptr<ResponseT>>> AsyncRequest(
      std::shared_ptr<RequestT> req, int64_t timeout_ms);

  // Fire-and-forget request (no response expected)
  // @param req: Request message
  // @return: 0 (kSuccess) on success, negative error code on failure
  int32_t RequestAndForget(std::shared_ptr<RequestT> req);

  // Deinitialize the proxy
  // @return: 0 (kSuccess) on success, negative error code on failure
  int32_t Deinit();

 private:
  void OnResponseReceived(const std::shared_ptr<const ResponseT>& resp);
  std::string GenerateCorrelationID();

  DDSParticipant* participant_;
  std::string instance_id_;
  std::string client_id_;  // UUID generated at Init
  std::atomic<uint32_t> sequence_;
  std::unordered_map<std::string, std::shared_ptr<ResponseT>> pending_;
  std::mutex mutex_;
  std::condition_variable cv_;
  bool initialized_;
  std::atomic<bool> service_online_;
};
```

---

## Usage Examples

### Server-Side Implementation

```cpp
// add_method_skeleton.h
#include "method/method_skeleton.h"
#include "proto/methods/add_method.pb.h"

class AddMethodSkeleton
    : public MethodSkeleton<AddRequestProto, AddResponseProto> {
 public:
  explicit AddMethodSkeleton(const std::string& instance_id = "");
  ~AddMethodSkeleton() override = default;

  int32_t Process(const std::shared_ptr<AddRequestProto>& req,
                  std::shared_ptr<AddResponseProto>& resp) override;
};

// add_method_skeleton.cc
AddMethodSkeleton::AddMethodSkeleton(const std::string& instance_id)
    : MethodSkeleton<AddRequestProto, AddResponseProto>(instance_id) {}

int32_t AddMethodSkeleton::Process(const std::shared_ptr<AddRequestProto>& req,
                                   std::shared_ptr<AddResponseProto>& resp) {
  // Validate request
  if (!req->has_a() || !req->has_b()) {
    return ToInt(MethodError::kUserErrorBase);  // -100: Missing fields
  }

  // Perform addition
  int32_t result = req->a() + req->b();
  resp->set_result(result);

  return ToInt(MethodError::kSuccess);  // 0
}

// Integration in ServerModule
class ServerModule {
 public:
  void OnSetUp(DDSParticipant* participant) {
    // Create skeleton with instance_id
    add_method_skeleton_ = std::make_shared<AddMethodSkeleton>("add_instance_1");

    // Start with 4 worker threads
    int32_t ret = add_method_skeleton_->Start(participant, 4);
    if (ret != 0) {
      LOG(ERROR) << "Failed to start AddMethodSkeleton: " << ret;
    } else {
      LOG(INFO) << "AddMethodSkeleton started with 4 worker threads";
    }
  }

  void OnShutdown() {
    if (add_method_skeleton_) {
      add_method_skeleton_->Stop();
    }
  }

 private:
  std::shared_ptr<AddMethodSkeleton> add_method_skeleton_;
};
```

### Client-Side Implementation

```cpp
// add_method_proxy.h
#include "method/method_proxy.h"
#include "proto/methods/add_method.pb.h"

class AddMethodProxy : public MethodProxy<AddRequestProto, AddResponseProto> {
 public:
  explicit AddMethodProxy(const std::string& instance_id = "");
  ~AddMethodProxy() override = default;
};

// add_method_proxy.cc
AddMethodProxy::AddMethodProxy(const std::string& instance_id)
    : MethodProxy<AddRequestProto, AddResponseProto>(instance_id) {}

// Integration in ClientModule
class ClientModule {
 public:
  void OnSetUp(DDSParticipant* participant) {
    // Create proxy with matching instance_id
    add_method_proxy_ = std::make_shared<AddMethodProxy>("add_instance_1");

    int32_t ret = add_method_proxy_->Init(participant);
    if (ret != 0) {
      LOG(ERROR) << "Failed to initialize AddMethodProxy: " << ret;
    } else {
      LOG(INFO) << "AddMethodProxy initialized successfully";
    }
  }

  void TestAddMethod() {
    // Create request
    auto req = std::make_shared<AddRequestProto>();
    req->set_a(42);
    req->set_b(58);

    LOG(INFO) << "Sending request - a=" << req->a() << ", b=" << req->b();

    // Create response
    auto resp = std::make_shared<AddResponseProto>();

    // Make RPC call with 5 second timeout
    int32_t status = add_method_proxy_->Request(req, resp, 5000);

    // Log response
    if (status == ToInt(MethodError::kSuccess)) {
      LOG(INFO) << "Response received - result=" << resp->result()
                << " (expected: " << (req->a() + req->b()) << ")";
    } else if (status == ToInt(MethodError::kTimeout)) {
      LOG(ERROR) << "Request timed out";
    } else {
      LOG(ERROR) << "Request failed with status: " << status;
    }
  }

  ~ClientModule() {
    if (add_method_proxy_) {
      add_method_proxy_->Deinit();
    }
  }

 private:
  std::shared_ptr<AddMethodProxy> add_method_proxy_;
};
```

---

## Core Implementation Details

### 1. UUID Generation and Correlation ID

```cpp
// method/method_impl.cc
namespace method_internal {

std::string GenerateUUID() {
  static boost::uuids::random_generator gen;  // Thread-safe
  boost::uuids::uuid uuid = gen();
  return boost::uuids::to_string(uuid);
  // Example: "550e8400-e29b-41d4-a716-446655440000"
}

int64_t GetCurrentTimeMicros() {
  auto now = std::chrono::system_clock::now();
  return std::chrono::duration_cast<std::chrono::microseconds>(
             now.time_since_epoch()).count();
}

}  // namespace method_internal

// In MethodProxy::Init()
client_id_ = method_internal::GenerateUUID();

// In MethodProxy::GenerateCorrelationID()
std::string MethodProxy::GenerateCorrelationID() {
  uint32_t seq = sequence_.fetch_add(1, std::memory_order_relaxed);
  return client_id_ + "_" + std::to_string(seq);
  // Example: "550e8400-e29b-41d4-a716-446655440000_1"
}
```

**Uniqueness Guarantee**:
- `client_id` uses boost::uuid v4 (2^122 possible values) - stored internally, not transmitted
- `sequence` uses atomic increment counter (thread-safe)
- `correlation_id` combines both, globally unique, collision probability < 10^-15
- **Optimization**: Only transmits `correlation_id`, not separate `client_id` and `sequence` (saves ~50 bytes/message)

### 2. Request-Response Correlation and Out-of-Order Handling

```cpp
// MethodProxy::Request() - Client side
int32_t Request(std::shared_ptr<RequestT> req,
                std::shared_ptr<ResponseT> resp,
                int64_t timeout_ms) {
  // 1. Generate unique correlation_id
  std::string correlation_id = GenerateCorrelationID();

  // 2. Fill request method_header
  auto* method_header = req->mutable_method_header();
  method_header->set_instance_id(instance_id_);
  method_header->set_correlation_id(correlation_id);
  method_header->set_fire_and_forget(false);

  // 3. Register pending request BEFORE publishing
  {
    std::lock_guard<std::mutex> lock(mutex_);
    pending_[correlation_id] = nullptr;  // Placeholder
  }

  // 4. Publish request via DDS
  auto result = participant_->Publish(*req);
  if (!result.ok()) {
    pending_.erase(correlation_id);
    return ToInt(MethodError::kPublishFailed);
  }

  // 5. Wait for response with timeout
  std::unique_lock<std::mutex> lock(mutex_);
  auto deadline = std::chrono::steady_clock::now() +
                  std::chrono::milliseconds(timeout_ms);

  bool received = cv_.wait_until(lock, deadline, [&]() {
    return pending_[correlation_id] != nullptr;
  });

  if (received) {
    *resp = *pending_[correlation_id];
    pending_.erase(correlation_id);
    return resp->method_header().status_code();
  }

  pending_.erase(correlation_id);
  return ToInt(MethodError::kTimeout);
}

// MethodProxy::OnResponseReceived() - Callback
void OnResponseReceived(const std::shared_ptr<const ResponseT>& resp) {
  std::string correlation_id = resp->method_header().correlation_id();

  std::lock_guard<std::mutex> lock(mutex_);

  // correlation_id is globally unique, so this lookup automatically filters:
  // - Our response: found in pending_ map
  // - Other client's response: not found, ignored
  auto it = pending_.find(correlation_id);
  if (it != pending_.end()) {
    it->second = std::make_shared<ResponseT>(*resp);
    cv_.notify_all();  // Wake up waiting thread
  }
  // else: Response for another client or timed-out request, ignore
}
```

**Out-of-Order Response Support**:
```
Timeline:
T0: Thread1 sends Request (uuid_1) -> pending_["uuid_1"] = nullptr
T1: Thread2 sends Request (uuid_2) -> pending_["uuid_2"] = nullptr
T2: Thread3 sends Request (uuid_3) -> pending_["uuid_3"] = nullptr

T3: Response 2 arrives (uuid_2) -> pending_["uuid_2"] = resp2, notify Thread2
T4: Response 3 arrives (uuid_3) -> pending_["uuid_3"] = resp3, notify Thread3
T5: Response 1 arrives (uuid_1) -> pending_["uuid_1"] = resp1, notify Thread1
```

### 3. Server Multi-threaded Processing

```cpp
// MethodSkeleton::Start()
int32_t Start(DDSParticipant* dds_participant, uint32_t thread_num) {
  participant_ = dds_participant;

  // Subscribe to RequestT messages (automatic type-based routing)
  participant_->Subscribe<RequestT>(&MethodSkeleton::OnRequestReceived, this, 10);

  // Create thread pool if multi-threaded
  if (thread_num >= 2) {
    thread_pool_ = std::make_unique<ThreadPool>(
        thread_num, nullptr, "MethodSkeleton", 0);
  }

  started_ = true;
  return ToInt(MethodError::kSuccess);
}

// MethodSkeleton::OnRequestReceived()
void OnRequestReceived(const std::shared_ptr<const RequestT>& req) {
  if (thread_pool_) {
    // Multi-threaded: Schedule to thread pool
    auto req_copy = std::make_shared<RequestT>(*req);
    thread_pool_->Schedule([this, req_copy]() {
      ProcessAndRespond(req_copy);
    });
  } else {
    // Single-threaded: Process inline
    ProcessAndRespond(req);
  }
}

// MethodSkeleton::ProcessAndRespond()
void ProcessAndRespond(const std::shared_ptr<const RequestT>& req) {
  // Filter by instance_id
  if (!instance_id_.empty() &&
      req->method_header().instance_id() != instance_id_) {
    return;
  }

  bool fire_and_forget = req->method_header().fire_and_forget();

  auto resp = std::make_shared<ResponseT>();

  // Copy method_header
  resp->mutable_method_header()->CopyFrom(req->method_header());

  // Call user-defined Process function
  int32_t status = Process(const_pointer_cast<RequestT>(req), resp);

  resp->mutable_method_header()->set_status_code(status);

  // Send response (unless fire-and-forget)
  if (!fire_and_forget) {
    participant_->Publish(*resp);
  }
}
```

### 4. Thread Safety Analysis

**MethodProxy Thread Safety**:
- `sequence_`: atomic variable, fetch_add guarantees thread safety
- `pending_`: mutex protected, all access within lock
- `cv_`: used with mutex, wait_until uses predicate to avoid spurious wakeups
- Multi-threaded concurrent Request: each request has unique correlation_id, no interference

**MethodSkeleton Thread Safety**:
- ThreadPool internally thread-safe (task queue has lock protection)
- OnRequestReceived can be called concurrently by multiple DDS threads
- Each request processed independently, no shared state conflicts

---

## Comparison with AUTOSAR ara::com and gRPC

### Feature Comparison Table

| Feature | DDS-based Method | AUTOSAR ara::com | gRPC |
|---------|------------------|------------------|------|
| **Transport Layer** | DDS (RTPS/SHM) | SOME/IP, DDS, Custom | HTTP/2 |
| **Message Format** | Protocol Buffers | SOME/IP Serialization, IDL | Protocol Buffers |
| **Service Discovery** | DDS Auto-discovery | Service Discovery (SD) | DNS, Consul, etcd |
| **Call Modes** | Sync/Async/Fire-and-forget | Fire&Forget, Request/Response, Event | Sync/Async/Stream |
| **Request-Response** | Correlation ID | Session/Request ID | HTTP/2 Stream ID |
| **Multi-Instance** | instance_id filtering | Instance Specifier | Multiple Service definitions |
| **Thread Model** | ThreadPool (Server) | EventLoop/Reactor | Thread Pool/Async |
| **Code Generation** | Manual template inheritance | ara_gen generates Proxy/Skeleton | protoc generates Stub |
| **Health Check** | IsMethodOnline() | FindService() | Health Check Service |
| **Timeout Control** | Client specified (ms) | Client specified | Client deadline |
| **Error Handling** | int32_t status code | ara::core::Result<T> | grpc::Status + Code |
| **Dependencies** | DDS + Boost.UUID | AUTOSAR Runtime | gRPC library + Protobuf |
| **Code Complexity** | Low (template inheritance) | Medium (generated code complexity) | Medium (Stub + Channel) |
| **Deployment Complexity** | Low (reuses infrastructure) | High (separate SOME/IP stack) | Medium (separate network stack) |
| **Performance (Latency)** | Low (SHM: <1ms, UDP: <10ms) | Low (SOME/IP: <5ms) | Medium (HTTP/2: 5-50ms) |
| **Cross-Language** | No (C++ only) | No (C++ only) | Yes (multi-language) |
| **Streaming** | No | Limited support | Full support |
| **Load Balancing** | No | No | Supported |
| **Use Case** | Intra-ECU communication | AUTOSAR vehicle systems | Cloud/cross-network communication |

### Detailed Comparison

#### 1. Architecture Design

**DDS-based Method**:
```cpp
// Based on existing DDS Pub/Sub, lightweight RPC abstraction
MethodProxy<ReqT, RespT> -> DDSParticipant::Publish<ReqT>()
                         <- DDSParticipant::Subscribe<RespT>()

MethodSkeleton<ReqT, RespT> <- DDSParticipant::Subscribe<ReqT>()
                             -> DDSParticipant::Publish<RespT>()

Advantages: Reuses existing infrastructure, no additional dependencies
Disadvantages: Limited to DDS transport layer
```

**AUTOSAR ara::com**:
```cpp
// AUTOSAR standard, independent transport layer
ara::com::Proxy -> ServiceRegistry -> SOME/IP/DDS
ara::com::Skeleton -> ServiceRegistry -> SOME/IP/DDS

Advantages: Standardized, supports multiple transport protocols
Disadvantages: Requires full AUTOSAR Runtime, complex deployment
```

**gRPC**:
```cpp
// Independent RPC framework based on HTTP/2
Stub -> Channel -> HTTP/2 -> Server

Advantages: Cross-platform, cross-language, rich ecosystem
Disadvantages: High network overhead, not suitable for real-time vehicle scenarios
```

#### 2. Message Structure

**DDS-based Method**:
```protobuf
message Request {
  MethodHeader method_header = 1;  // RPC metadata
  int32 user_field = 2;            // User payload
}
```

**AUTOSAR ara::com**:
```cpp
// SOME/IP Header
struct SomeIpHeader {
  uint16_t service_id;
  uint16_t method_id;
  uint32_t length;
  uint16_t client_id;
  uint16_t session_id;
  uint8_t protocol_version;
  uint8_t interface_version;
  uint8_t message_type;
  uint8_t return_code;
};
// + IDL serialized user data
```

**gRPC**:
```protobuf
// HTTP/2 Header
:method = POST
:scheme = http
:path = /package.Service/Method
:authority = server.example.com
content-type = application/grpc+proto
// + Protobuf serialized user data
```

#### 3. Performance Comparison (Typical Vehicle Scenarios)

| Scenario | DDS-based Method | ara::com (SOME/IP) | gRPC |
|----------|------------------|-------------------|------|
| Intra-ECU (SHM) | **0.5-1ms** | N/A | N/A |
| Inter-ECU (Ethernet) | 5-10ms (UDP) | **2-5ms** | 20-50ms |
| Cross-Domain (CAN/FlexRay) | Not supported | 10-100ms | N/A |
| Cross-Network (4G/5G) | Not supported | Not supported | **50-200ms** |

#### 4. Usage Complexity Comparison

**DDS-based Method** - Simple and intuitive
```cpp
// Server: 2 files (header + cc)
class MyMethodSkeleton : public MethodSkeleton<ReqT, RespT> {
  int32_t Process(...) override { /* implement business logic */ }
};
skeleton_->Start(participant, 4);  // Start with 4 threads

// Client: 2 files
class MyMethodProxy : public MethodProxy<ReqT, RespT> {};
proxy_->Init(participant);
proxy_->Request(req, resp, 5000);
```

**AUTOSAR ara::com** - Requires code generation
```cpp
// 1. Write ARXML service description
// 2. Run ara_gen to generate Proxy/Skeleton code (thousands of lines)
// 3. Write business logic
auto find_result = ara::com::FindService(...);
auto proxy = find_result->GetResult();
proxy->Method(args).get();  // Call
```

**gRPC** - Requires code generation and Channel management
```cpp
// 1. Write .proto file
// 2. Run protoc to generate Stub code
// 3. Create Channel
auto channel = grpc::CreateChannel("localhost:50051", ...);
auto stub = MyService::NewStub(channel);
stub->Method(&context, request, &response);
```

#### 5. Use Case Recommendations

**Recommended for DDS-based Method**:
- Intra-ECU module communication (low latency, high frequency calls)
- Already have DDS Pub/Sub infrastructure
- Rapid prototyping and validation
- No cross-language/cross-platform support needed
- No strict AUTOSAR standard requirements

**Recommended for AUTOSAR ara::com**:
- Need AUTOSAR standard compliance
- Inter-ECU SOME/IP communication
- Need Service Discovery
- Multi-ECU distributed systems
- Need toolchain support (ara_gen)

**Recommended for gRPC**:
- Vehicle-cloud communication (T-Box, OTA)
- Backend service development
- Cross-language system integration
- Need streaming (video streams, log streams)
- Microservice architecture

---

## Advantages and Limitations

### Advantages

1. **Lightweight Integration**
   - Reuses existing DDS Pub/Sub infrastructure, no additional transport layer
   - Concise code, easy to understand and maintain
   - Few compilation dependencies (only DDS + Boost.UUID)

2. **High Performance**
   - SHM transport latency < 1ms
   - Zero-copy support (shared memory scenarios)
   - ThreadPool concurrent processing, fully utilizes multi-core

3. **Type Safety**
   - Compile-time type checking (templates)
   - Protobuf strongly-typed messages
   - No runtime type conversion overhead

4. **Flexibility**
   - Supports Sync/Async/Fire-and-forget three modes
   - Configurable timeout
   - Supports multi-instance (instance_id)

5. **Easy Debugging**
   - Clear log output (correlation_id, client_id)
   - Traceable request chain
   - Explicit status codes

### Limitations

1. **Depends on DDS**
   - Only applicable to systems with DDS infrastructure
   - Cannot be used independently from existing infrastructure

2. **No Cross-Language Support**
   - Only supports C++
   - Cannot interoperate with Python/Java/Go etc.

3. **No Streaming**
   - Does not support Server Stream, Client Stream, Bidirectional Stream
   - Only supports one-shot request-response

4. **Limited Service Discovery**
   - IsMethodOnline() is optimistic assumption only
   - No active heartbeat detection
   - No service version management

5. **No Load Balancing**
   - Single instance communication
   - No multi-replica service support
   - No automatic failover

---

## Best Practices

### 1. Error Handling

```cpp
// Server side
int32_t Process(const std::shared_ptr<AddRequestProto>& req,
                std::shared_ptr<AddResponseProto>& resp) {
  // Validate input
  if (!req->has_a() || !req->has_b()) {
    return ToInt(MethodError::kUserErrorBase);  // -100
  }

  // Business logic
  try {
    resp->set_result(req->a() + req->b());
    return ToInt(MethodError::kSuccess);  // 0
  } catch (const std::exception& e) {
    LOG(ERROR) << "Processing failed: " << e.what();
    return ToInt(MethodError::kUserErrorBase) - 1;  // -101
  }
}

// Client side
int32_t status = proxy_->Request(req, resp, 5000);
if (status == ToInt(MethodError::kSuccess)) {
  // Success
} else if (status == ToInt(MethodError::kTimeout)) {
  // Retry or fallback
} else if (status < ToInt(MethodError::kUserErrorBase)) {
  // Server error
  LOG(ERROR) << "Server error: " << status;
}
```

### 2. Timeout Settings

```cpp
// Short timeout: fail fast
proxy_->Request(req, resp, 100);  // 100ms

// Medium timeout: normal RPC
proxy_->Request(req, resp, 5000);  // 5s

// Long timeout: time-consuming operations
proxy_->Request(req, resp, 60000);  // 60s
```

### 3. Async Calls

```cpp
// Fire multiple requests
auto future1 = proxy_->AsyncRequest(req1, 5000);
auto future2 = proxy_->AsyncRequest(req2, 5000);
auto future3 = proxy_->AsyncRequest(req3, 5000);

// Wait for all
auto [status1, resp1] = future1.get();
auto [status2, resp2] = future2.get();
auto [status3, resp3] = future3.get();
```

### 4. Resource Management

```cpp
// Always call Deinit() in destructor
~MyModule() {
  if (proxy_) {
    proxy_->Deinit();  // Cleanup pending requests
  }
  if (skeleton_) {
    skeleton_->Stop();  // Drain thread pool
  }
}
```

### 5. Logging

```cpp
void TestMethod() {
  LOG(INFO) << "Sending request - a=" << req->a() << ", b=" << req->b();

  int32_t status = proxy_->Request(req, resp, 5000);

  if (status == ToInt(MethodError::kSuccess)) {
    LOG(INFO) << "Response received - result=" << resp->result();
  } else {
    LOG(ERROR) << "Request failed - status=" << status;
  }
}
```

---

## Summary

The DDS-based Method framework provides a lightweight RPC solution built on existing Pub/Sub infrastructure:

- **vs ara::com**: More lightweight, easy integration, suitable for intra-ECU communication, but not AUTOSAR compliant
- **vs gRPC**: Higher performance, lower latency, but limited to C++ and existing infrastructure
- **Best Scenario**: Intra-vehicle ECU module communication requiring low latency and simple integration

### Protocol Optimization

The MethodHeader has been optimized for efficiency:

**Optimized Field Design**:
- `instance_id` - Server instance routing
- `correlation_id` - Request-response correlation (format: `{uuid}_{seq}`)
- `fire_and_forget` - Control flag
- `status_code` - Response status code
- Optional timestamp and sequence fields for DDS metadata

**Benefits**:
- Saves ~100 bytes per request-response pair (50 bytes/message × 2)
- Clean protocol design
- Reduced maintenance cost, avoids field duplication
- No performance impact (hash map lookup vs string comparison, difference <10ns)

**Selection Guide**:
- Intra-ECU communication → **DDS-based Method**
- Inter-ECU SOME/IP → **ara::com**
- Vehicle-cloud communication → **gRPC**
