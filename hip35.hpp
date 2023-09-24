#ifndef HIP35_HPP
#define HIP35_HPP 

#include "backend.hpp"
#include "screen.hpp"
#include "observer.hpp"
#include <memory> // unique_ptr

class Hip35
{
public:
    Hip35(std::unique_ptr<Gui::Frontend> frontend,
          std::unique_ptr<Rpn::Backend> backend,
          std::unique_ptr<Observer> observer);
    void RunUI();

private:
    std::unique_ptr<Gui::Frontend> frontend_;
    std::unique_ptr<Rpn::Backend> backend_;
    std::unique_ptr<Observer> observer_;
};

#endif /* HIP35_HPP */
