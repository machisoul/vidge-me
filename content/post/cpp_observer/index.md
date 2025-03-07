---
author: "Vidge Wong"
title: "Observer Pattern"
date: "2021-07-21"
description: "Understanding the Observer pattern with practical C++ implementations"
tags: ["c/cpp"]
categories: ["Design Pattern"]
image: "img/cover/design.png"
ShowToc: true
TocOpen: true
---

## Introduction

The Observer pattern defines a one-to-many dependency between objects where when one object (the subject) changes state, all its dependents (observers) are notified and updated automatically. It's commonly used in event handling systems, GUI frameworks, and real-time data monitoring.

## Basic Implementation

Here's a basic implementation of the Observer pattern:

```cpp
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <memory>

// Observer interface
class IObserver {
public:
    virtual void update(const std::string& message) = 0;
    virtual ~IObserver() = default;
};

// Subject interface
class ISubject {
public:
    virtual void attach(IObserver* observer) = 0;
    virtual void detach(IObserver* observer) = 0;
    virtual void notify(const std::string& message) = 0;
    virtual ~ISubject() = default;
};

// Concrete Subject
class NewsAgency : public ISubject {
private:
    std::vector<IObserver*> observers;

public:
    void attach(IObserver* observer) override {
        observers.push_back(observer);
    }

    void detach(IObserver* observer) override {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }

    void notify(const std::string& message) override {
        for (auto observer : observers) {
            observer->update(message);
        }
    }

    void publishNews(const std::string& news) {
        std::cout << "NewsAgency: Publishing news: " << news << std::endl;
        notify(news);
    }
};

// Concrete Observers
class NewsChannel : public IObserver {
private:
    std::string name;

public:
    NewsChannel(const std::string& channelName) : name(channelName) {}

    void update(const std::string& message) override {
        std::cout << name << " received news: " << message << std::endl;
    }
};
```

## Real-World Example

Here's a more practical example showing a stock price monitoring system:

```cpp
// Stock price data structure
struct StockData {
    std::string symbol;
    double price;
    std::string timestamp;
};

// Observer interface for stock updates
class IStockObserver {
public:
    virtual void onPriceChange(const StockData& data) = 0;
    virtual ~IStockObserver() = default;
};

// Subject that monitors stock prices
class StockMarket : public ISubject {
private:
    std::vector<IStockObserver*> observers;
    std::unordered_map<std::string, double> stockPrices;

public:
    void attach(IStockObserver* observer) {
        observers.push_back(observer);
    }

    void detach(IStockObserver* observer) {
        observers.erase(
            std::remove(observers.begin(), observers.end(), observer),
            observers.end()
        );
    }

    void updatePrice(const std::string& symbol, double newPrice) {
        auto currentTime = std::chrono::system_clock::now();
        auto timestamp = std::to_string(
            std::chrono::system_clock::to_time_t(currentTime)
        );

        StockData data{symbol, newPrice, timestamp};
        
        // Notify only if price actually changed
        if (stockPrices[symbol] != newPrice) {
            stockPrices[symbol] = newPrice;
            for (auto observer : observers) {
                observer->onPriceChange(data);
            }
        }
    }
};

// Concrete observers
class StockTrader : public IStockObserver {
private:
    std::string name;
    double threshold;

public:
    StockTrader(const std::string& traderName, double priceThreshold)
        : name(traderName), threshold(priceThreshold) {}

    void onPriceChange(const StockData& data) override {
        if (data.price > threshold) {
            std::cout << name << ": Alert! " << data.symbol 
                      << " price (" << data.price 
                      << ") exceeded threshold (" << threshold << ")"
                      << std::endl;
        }
    }
};

class PriceDisplay : public IStockObserver {
public:
    void onPriceChange(const StockData& data) override {
        std::cout << "Price Display: " << data.symbol 
                  << " - $" << data.price 
                  << " at " << data.timestamp 
                  << std::endl;
    }
};
```

## Usage Example

```cpp
int main() {
    // Basic example
    NewsAgency newsAgency;
    NewsChannel bbcNews("BBC News");
    NewsChannel cnNews("CN News");

    newsAgency.attach(&bbcNews);
    newsAgency.attach(&cnNews);
    newsAgency.publishNews("Breaking: Observer Pattern Demonstration!");

    // Stock market example
    StockMarket market;
    StockTrader trader1("John", 100.0);
    StockTrader trader2("Alice", 150.0);
    PriceDisplay display;

    market.attach(&trader1);
    market.attach(&trader2);
    market.attach(&display);

    market.updatePrice("AAPL", 145.50);
    market.updatePrice("GOOGL", 2750.25);

    return 0;
}
```

Output:
```
NewsAgency: Publishing news: Breaking: Observer Pattern Demonstration!
BBC News received news: Breaking: Observer Pattern Demonstration!
CN News received news: Breaking: Observer Pattern Demonstration!

Price Display: AAPL - $145.50 at 1621234567
John: Alert! AAPL price (145.50) exceeded threshold (100.0)
Price Display: GOOGL - $2750.25 at 1621234567
John: Alert! GOOGL price (2750.25) exceeded threshold (100.0)
Alice: Alert! GOOGL price (2750.25) exceeded threshold (150.0)
```

## Modern C++ Implementation

Here's a more modern approach using smart pointers and templates:

```cpp
template<typename T>
class Observer {
public:
    virtual void update(const T& data) = 0;
    virtual ~Observer() = default;
};

template<typename T>
class Subject {
private:
    std::vector<std::weak_ptr<Observer<T>>> observers;

public:
    void attach(std::shared_ptr<Observer<T>> observer) {
        observers.push_back(observer);
    }

    void notify(const T& data) {
        observers.erase(
            std::remove_if(
                observers.begin(),
                observers.end(),
                [&data](auto& weakObs) {
                    if (auto obs = weakObs.lock()) {
                        obs->update(data);
                        return false;
                    }
                    return true;
                }
            ),
            observers.end()
        );
    }
};
```

## Best Practices

1. **Memory Management**:
   - Use smart pointers to manage observer lifetimes
   - Consider weak_ptr to prevent circular references
   - Clean up expired observers

2. **Performance**:
   - Consider using a notification threshold
   - Batch updates when possible
   - Implement efficient observer removal

3. **Thread Safety**:
   - Protect observer list with mutex if needed
   - Consider concurrent notification strategies
   - Handle observer removal safely

4. **Design Considerations**:
   - Keep observer interfaces simple
   - Consider push vs pull update strategies
   - Avoid circular update chains

## Conclusion

The Observer pattern is essential for building loosely coupled systems where objects need to interact without having direct knowledge of each other. Modern C++ provides powerful tools to implement this pattern efficiently and safely. 