#ifndef OBSERVER_HPP
#define OBSERVER_HPP 

#include <string>
#include <iostream>
#include <tuple> // make_pair, tuple


// observer's interface
class IObserver {
public:
    virtual void UpdateValue(double value) = 0;
    virtual void UpdateOperation(const std::string& operation) = 0;
    virtual std::tuple<std::string, double> GetState() const = 0;
};

class Observer: public IObserver {
    public:
        Observer():
            data_(0.0),
            operation_("") {}
        ~Observer() {}
        virtual void UpdateValue(double value);
        virtual void UpdateOperation(const std::string& operation);
        virtual std::tuple<std::string, double> GetState() const {
           return std::make_pair(operation_, data_);
        }


    private:
        // the observer mimics some of Backend's private data
        double data_;
        std::string operation_;
};

#endif /* OBSERVER_HPP */
