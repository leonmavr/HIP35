#ifndef BACKEND_HPP
#define BACKEND_HPP 

#include "base.hpp"
#include "observer.hpp"
#include "stack.hpp"
#include "keypad.hpp"
#include <string> // string
#include <memory> // unique_ptr
#include <cmath> // sin, cos, tan, log10, sqrt
#include <stdexcept> // runtime_error
#include <vector> // vector
#include <utility> // make_pair, pair
#include <array> // array 

/**
 * @brief Subject class to observe in the observer design pattern.
 *        After creating an instance, "attach" this to the class to
 *        observe by the `Attach` method.
 *        This class is meant to be inherited by the class to observe.
 *        We'll call the derived class that inherits it subject.
 *        Inside the methods to observe, run the Subject's Notify<...>
 *        methods.
 *        This class HAS a bunch of observers that are notified about
 *        the subject's changes. Through the Notify methods, it helps
 *        the observer record the state of the subject. Therefore you
 *        need to modify the Observer's update/get methods accordingly.
 */
class Subject {
public:
    void Attach(Observer* observer) { observers_.push_back(observer); }
    // Remove the observer from the list
    void Detach(Observer* observer);

protected:
    // Meant to be integrated with derived class's methods when a value
    // is inserted
    void NotifyValue(std::pair<double, double> registers);
    // Meant to be integrated with derived class's methods when an
    // operation is executed
    void NotifyOperation(const std::string& operation);

private:
    // we need a list of observes in order to observe multiple instances
    // (if necessary)
    std::vector<Observer*> observers_;
};


namespace Rpn {

/**
* @brief Implements a reverse Polish notation (RPN) calculator [1].
*        The architecture more or less follows the basic architecture
*        of the HP35 calculator. It contains a stack of 4 registers;
*        X, Y, Z and T (T for top, X for bottom). Therefore when we store
*        a number, it's written to the X register. When we query data
*        from the stack, the first item to be removed is X. It also
*        contains a LASTX register which stores the last value of X
*        before a function button is pressed. Its purpose is to correct
*        mistakes. The calculator supports the following basic operations
*        blueprinted in the `IBackend` class:
*        -- swapXY
*        -- Peek
*        -- Insert
*        -- RND (Rotate down)
*        -- Enter
*        -- Calculate (calculator's function buttons)
*        RPN notation heavily relies on the stack. Operands are pushed
*        on a stack. An operation (e.g. +, SIN, etc.) always follows
*        its operand(s) and when it's complete the top of the stack
*        gets overwritten with its result.
*        Below is an example to give you some intuition:
*
*        4 - (3 + 2) or in RPN; we do 3+2 first, then 4 - 5:
*        4 3 2 + -
*          |     |    |     |    |  4  |   |     |   |     |
*        4 |     | 3  |  4  | 2  |  3  | + |  4  | - |     | <- Y
*          |  4  |    |  3  |    |  2  |   |  5  |   | -1  | <- X
*          +-----+    +-----+    +-----+   +-----+   +-----+ 
*
*        More details in each method's documentation.
*
*        Inherits from:
*        -- IBackend; to implement its abstract methods
*        -- Subject; to be an observable subject by the Observer class
*
*        References:
*        -----------
*        [1] "Enter: Reverse Polish Notation Made Easy" by J. Dodin
*            https://literature.hpcalc.org/community/enter-en.pdf
*/
class Backend: public IBackend, public Subject {
public:
    Backend() = delete;
    Backend(const Key::Keypad& keypad);
    Backend(const Backend& other) :
        stack_(std::make_unique<Stack>(*other.stack_)),
        do_shift_up_(other.do_shift_up_),
        lastx_(other.lastx_),
        keypad_(other.keypad_),
        sto_regs_(other.sto_regs_) {}
    ~Backend() {}
    /**
     * @brief Swaps values of registers X and Y.
     */
    void SwapXY() override;
    /**
     * @brief Gets current value of register X.
     *
     * @return Pair of values at registers X and Y
     */
    std::pair<double, double> Peek() const override {
        return std::make_pair((*stack_)[IDX_REG_X],
                              (*stack_)[IDX_REG_Y]);
    }
    /**
     * @brief Insert a number in the stack by writing to register
     *        X. Remember that each RPN ends with a(n) operation(s).
     *        Therefore this method lifts the stack up before
     *        writing in it if a calculation previously took place.
     *        Example:
     *        Initially: X = 6, Y = 6
     *        Type: 9 + 13
     *        (L means lift the stack, aka shift up)
     *        |     |     |     |     |     |      |     |
     *        |  6  |  9  |  6  |  +  |     |  13  |  15 | 
     *        |  6  |     |  9  |     |  15 |      |  13 |
     *        +-----+     +-----+     +-----+      +-----+
     *           L = false      L = true     L = false
     * @param num Decimal number to insert.
     */
    void Insert(double num) override;
    /**
     * @brief Circularly rotates the stack down, e.g.:
     *           before:     after:    |
     *        T->   4          1       |   t ----------+  T <-+
     *        Z->   3          4       |   z -------+  +> Z   |
     *        Y->   2          3       |   y ----+  +---> Y   |
     *        X->   1          2       |   x     +------> X   |
     *                                 |   -------------------+
     */                                
    void Rdn() override;
    /**
     * @brief Emulate enter key; shift the stack up, discarding T
     *        register and then clone Y into X, e.g.:
     *           before:     after:    |
     *        T->   4          3       |   t   +----------> T
     *        Z->   3          2       |   z --+  +-------> Z
     *        Y->   2          1       |   y -----+  +----> Y
     *        X->   1          1       |   x --------+----> X
     */
    void Enter() override;
    /**
    * @brief The LASTX register is a companion to the stack: it holds
    *        holds the number that was in the X–register before the
    *        last numeric function (e.g. SQRT or +) was executed.
    *        Pressing LASTX inserts the value of LASTX in the stack.
    *        1. Reusing a number in a calculation
    *           Suppose we want to compute sin(6.24) * tan(6.24/2)
    *           by pressing 6.24 SIN 6.24 2 / TAN *
    *           6.24 was written to LASTX when pressing SIN so it
    *           can be reused in the following way:
    *           6.24 SIN LASTX 2 / TAN *
    *        2. Correcting errors.
    *           2a) Single-argument functions
    *               Suppose we typed 6.24 SIN 6.24 2 / COS instead 
    *               of 6.24 SIN 6.24 2 / TAN. We can correct it w/o
    *               erasing the whole thing. Before pressing COS,
    *               LASTX will store 6.24/2 so to correct it use:
    *               RDN   (get rid of cos(6.24/2)) X -> g sin(6.24))
    *               LASTX (insert LASTX 6.24/2)    X -> 6.24/2 
    *                                              Y -> sin(6.24)
    *               TAN                            X -> tan(6.24/2)  
    *                                              Y -> sin(6.24) 
    *               *       X -> sin(6.24) * tan(6.24/2) = 0.00093 
    *           2b) 2-argument function and wrong function
    *               Suppose we want to calculate 20*23  (20 ENT 23 *)
    *               but typed 20 ENT 23 -. Correct it as follows:
    *               20 ENT 23 - . . . . . . . . . X -> 20 - 23
    *               LASTX . . . . . . . . . . . . X -> 23
    *                                             Y -> 20 - 23
    *               + (undo last operation) . . . X -> 20
    *               23 . . . . . . . . . . . . . .X -> 23
    *                                             Y -> 20
    *               * . . . . . . . . . . . . . . X -> 460
    *           2c) Wrong first number (Y register)
    *               19 ENT 23 * . . . . . . . . . X -> 440 
    *               20 . . . . . . . . . . . . . .X -> 20
    *                                             Y -> 440 
    *               LASTX. . . . . . . . . . . .  X -> 23
    *                                             Y -> 20
    *                                             Z -> 440 
    *               * . . . . . . . . . . . . . . X -> 460
    *                                             Y -> 440 
    *               Before starting, we can optionally RND to discard 440.
    *           2d) Wrong second number (X register)
    *               20 ENT 22 * . . . . . . . . . X -> 22 * 20
    *               LASTX . . . . . . . . . . . . X -> 22
    *                                             Y -> 22 * 20
    *               / . . . . . . . . . . . . . . X -> 20
    *               23 . . . . . . . . . . . . . .X -> 23
    *                                             Y -> 20
    *               * . . . . . . . . . . . . . . X -> 23 * 20
    */
    void LastX() override;
    /**
     * @brief Emulate pressing a function key. Function keys operate
     *        either on the X register (1-operand key, e.g. LOG) or
     *        on the X and Y registers (2-operand keys, e.g. +).
     *        If an 1-op calculation is entered, use register X as
     *        input and output. If a 2-op calculation is entered, use
     *        reg. X, Y as inputs, write output to Y and shift down
     *        the stack. Example:
     *           before: (LOG)  after:  |  before:    (/)    after:
     *        T->   4             3     |     4                4
     *        Z->   3             2     |     3                4
     *        Y->   2             1     |     2                3
     *        X->   1             0     |    0.1               20
     *
     *        t->   ------------> T     |  t->   ----------+-> T
     *        z->   ------------> Z     |  z->   ------+   +-> Z
     *        y->   ------------> Y     |  y->   ---+  +-----> Y
     *        x->   -----f(x)---> X     |  x->   ---+-f(x,y)-> X
     *         
     * @param operation What numerical operation to perform. This
     *                  can be one of the keys of function_key_1op_ 
     *                  or function_key_2op_ - see `IBackend` class
     *
     * @return The calculation's result 
     */
    double Calculate(std::string operation) override;
    /**
     * @brief Calculte a results of an expression written in reverse
     *        Polish notation (RPN). In RPN only records operations 
     *        and operands and each operation follows its operands.
     *        Example: 12 7 - gives 5
     *                 6 2 / COS gives COS(6/2)
     *
     * @param rpnExpression A string that contains a RPN notation
     *                      expression. Operands and operations are
     *                      separated by space. Variables are not
     *                      allowed.
     *
     * @return The numberic results of the RPN. 
     */
    double CalculateFromString(std::string rpnExpression) override;

    /**
     * @brief Set register X to zero. The purpose of this is to
     *        fix typos and the last entered number.
     */
    void Clx() override;
    /* Set all registers (entire stack) to zero */
    void Cls() override;
    /* Insert the value of PI to register X */
    void Pi() override;
    void Sto(std::size_t idx) override;
    void Rcl(std::size_t idx) override;
    void DoStackOperation(const std::string& operation);
    /* Overrides the << operator for the class, e.g.std::cout << <Instance>; */
    friend std::ostream& operator<<(std::ostream& os, const Backend& backend);

private:
    // owns the stack - unique_ptr manages its lifetime and deallocation
    std::unique_ptr<Stack> stack_;
    /**
     * @brief General purpose storage registers (indexed 0 to 9) to
     *        store constants or intermediate results.
     */
    std::array<double, 10> sto_regs_;
    // LASTX register; stores the value of X before a function is invoked 
    double lastx_;
    // shift up (lift) stack after an operation is executed (e.g. +, -, etc.)
    // to make space for a new operand
    bool do_shift_up_;
    // reference to a keypad that describes the calculator's key configuration
    const Key::Keypad& keypad_;
    /**
     * @brief Maps a key description back to input key, e.g.
     *        LOG10 -> L. The purpose of this is to be able to
     *        convert functions in non-interactive mode (parsing
     *        from string) into functions understood by the class,
     *        e.g. 2 10 LOG10 -> 2 10 l. Then, "l" is mapped to an
     *        actual function (std::functional) and log10(10) takes
     *        place.
     */
    std::unordered_map<std::string, std::string> reverse_keys_;
};


} /* namespace Rpn */

#endif /* BACKEND_HPP */
