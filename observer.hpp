#ifndef OBSERVER_HPP
#define OBSERVER_HPP 

#include <string>
#include <iostream>
#include <tuple> // make_pair, tuple


// observer's interface
class IObserver {
public:
    virtual void UpdateOperation(const std::string& operation) = 0;
    virtual void UpdateRegisters(std::pair<double, double> registers) = 0;
    virtual std::pair<std::string, std::pair<double, double>> GetState() const = 0;
};

class Observer: public IObserver {
    public:
        Observer():
            data_(std::make_pair<double, double>(0, 0)),
            operation_("") {}
        ~Observer() {}
        virtual void UpdateRegisters(std::pair<double, double> registers);
        virtual void UpdateOperation(const std::string& operation);
        virtual std::pair<std::string, std::pair<double, double>> GetState() const {
           return std::make_pair(operation_, data_);
        }


    private:
        // the observer mimics some of Backend's private data
        std::pair<double, double> data_;
        std::string operation_;
};

#endif /* OBSERVER_HPP */
