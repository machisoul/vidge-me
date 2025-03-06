---
author: "Vidge Wong"
title: "Singleton Pattern"
date: "2021-05-19"
summary: "A comprehensive guide to implementing the Singleton pattern in C++, covering different approaches and best practices"
tags: ["c/cpp"]
categories: ["Design Pattern"]
cover:
  image: images/cover/default.jpg
  hiddenInList: true
ShowToc: true
TocOpen: true
---

## Introduction

The Singleton pattern is one of the most commonly used design patterns in software development. It ensures that a class has only one instance throughout the application's lifecycle and provides a global point of access to that instance. While the pattern is sometimes criticized for potentially creating global state, there are legitimate use cases where it's valuable, such as managing shared resources or configuration settings.

## Implementation Approaches

### 1. Basic Non-Thread-Safe Implementation

This is the simplest implementation but should not be used in multi-threaded applications:

```cpp
class Singleton {
private:
    static Singleton* instance;
    Singleton() = default;

public:
    static Singleton* getInstance() {
        if (instance == nullptr) {
            instance = new Singleton();
        }
        return instance;
    }
};

Singleton* Singleton::instance = nullptr;
```

### 2. Thread-Safe with Mutex Lock

A straightforward thread-safe implementation using mutex:

```cpp
class Singleton {
private:
    static Singleton* instance;
    static std::mutex mutex;
    Singleton() = default;

public:
    static Singleton* getInstance() {
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = new Singleton();
        }
        return instance;
    }
};

Singleton* Singleton::instance = nullptr;
std::mutex Singleton::mutex;
```

### 3. Double-Checked Locking (Pre-C++11)

This pattern was popular before C++11 to improve performance, but it's prone to subtle errors:

```cpp
class Singleton {
private:
    static Singleton* instance;
    static std::mutex mutex;
    Singleton() = default;

public:
    static Singleton* getInstance() {
        if (instance == nullptr) {  // First check
            std::lock_guard<std::mutex> lock(mutex);
            if (instance == nullptr) {  // Second check
                instance = new Singleton();
            }
        }
        return instance;
    }
};
```

### 4. Meyers' Singleton (Modern C++ Approach)

This is the recommended approach for modern C++ (C++11 and later):

```cpp
class Singleton {
public:
    static Singleton& getInstance() {
        static Singleton instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

private:
    Singleton() = default;
    ~Singleton() = default;
};
```

## Best Practices and Considerations

### 1. Thread Safety

- Use Meyers' Singleton for automatic thread-safe initialization
- Avoid double-checked locking pattern in modern C++
- If using explicit locking, consider the performance impact

### 2. Memory Management

```cpp
class Singleton {
public:
    static Singleton& getInstance() {
        static std::unique_ptr<Singleton> instance(new Singleton());
        return *instance;
    }
    
private:
    Singleton() = default;
};
```

### 3. Destruction Order

Consider using the Nifty Counter Idiom for better control over destruction order:

```cpp
class Singleton {
private:
    struct Deleter {
        ~Deleter() {
            if (Singleton::instance != nullptr) {
                delete Singleton::instance;
                Singleton::instance = nullptr;
            }
        }
    };
    
    static Singleton* instance;
    static Deleter deleter;
};
```

### 4. Testing Considerations

Make your Singleton testable by using dependency injection:

```cpp
class SingletonInterface {
public:
    virtual ~SingletonInterface() = default;
    virtual void doSomething() = 0;
};

class Singleton : public SingletonInterface {
public:
    static void setInstance(std::unique_ptr<SingletonInterface> newInstance) {
        instance = std::move(newInstance);
    }
    
    static SingletonInterface& getInstance() {
        if (!instance) {
            instance = std::make_unique<Singleton>();
        }
        return *instance;
    }
    
private:
    static std::unique_ptr<SingletonInterface> instance;
};
```

## Common Use Cases

1. **Configuration Management**
   ```cpp
   class ConfigManager {
   public:
       static ConfigManager& getInstance() {
           static ConfigManager instance;
           return instance;
       }
       
       void loadConfig(const std::string& path) {
           // Load configuration
       }
       
       const std::string& getValue(const std::string& key) const {
           return config[key];
       }
       
   private:
       std::unordered_map<std::string, std::string> config;
   };
   ```

2. **Logger Systems**
   ```cpp
   class Logger {
   public:
       static Logger& getInstance() {
           static Logger instance;
           return instance;
       }
       
       void log(const std::string& message) {
           std::lock_guard<std::mutex> lock(mutex);
           std::cout << "[" << getCurrentTime() << "] " << message << std::endl;
       }
       
   private:
       std::mutex mutex;
   };
   ```

## Conclusion

While the Singleton pattern can be useful in specific scenarios, it should be used judiciously. Modern C++ provides excellent tools for implementing thread-safe Singletons, with Meyers' Singleton being the preferred approach. Always consider whether a Singleton is truly necessary for your use case, as global state can make testing and maintenance more difficult.

Remember to:
- Use modern C++ features when possible
- Consider thread safety requirements
- Plan for proper cleanup
- Make your Singletons testable
- Document the rationale for using a Singleton

The pattern remains valuable for specific use cases like configuration management, logging systems, and resource pools, but should not be overused. 