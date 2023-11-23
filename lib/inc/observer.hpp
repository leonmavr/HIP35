#ifndef OBSERVER_HPP
#define OBSERVER_HPP 

#include <string>
#include <iostream>
#include <tuple> // make_pair, tuple


/**
* @brief Observer class' interface. Blueprint for observer's
*        basic methods:
*        - UpdateOperation - records last operation entered
*        - UpdateRegisters - records last registers X, Y 
*/
class IObserver {
public:
    IObserver() {};
    virtual ~IObserver() {};
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
        void UpdateRegisters(std::pair<double, double> registers) override;
        void UpdateOperation(const std::string& operation) override;
        std::pair<std::string, std::pair<double, double>> GetState() const override {
           return std::make_pair(operation_, data_);
        }


    private:
        // the observer mimics some of Backend's private data
        std::pair<double, double> data_;
        std::string operation_;
};

#endif /* OBSERVER_HPP */
