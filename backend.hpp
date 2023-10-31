#ifndef BACKEND_HPP
#define BACKEND_HPP 

#include "ibackend.hpp"
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
* @brief The type of the current token - helps determine what
*        function to execute.
*/
typedef enum {
    kTypeNone = 0,       // how the clas is initialized
    kTypeOperand,        // essentially number
    kTypeStack,          // e.g. SWAP, RDN
    kTypeNumeric,        // e.g. +, SIN
    kTypeEnter,          // ENTER key 
    kTypeStorage         // storage, load in general registers
} TokenType;

/**
 * @brief Various flags set during the calculator's operation.
 */
typedef struct Flags {
    /** @brief Shift up the stack to make space for new entries */
    bool shift_up;
    /** @brief EEX key pressed*/
    bool eex_pressed;
    /** @brief RCL or STO key pressed*/
    bool rcl_sto_pressed;
} Flags;

/**
* @brief Implements a reverse Polish notation (RPN) calculator [1].
*        The architecture more or less follows the basic architecture
*        of the HP35 calculator. It contains a stack of 4 registers;
*        X, Y, Z and T (T for top, X for bottom). Therefore when we store
*        a number, it's written to the X register. It also contains
*        a LASTX register which stores the last value of X before a
*        function button is pressed.The calculator supports the
*        following basic operations blueprinted in the `IBackend`
*        class such as:
*        - `SwapXY`
*        - `Peek`
*        - `Insert`
*        - `Rdn` (Rotate down)
*        - `Enter`
*
*        RPN notation heavily relies on the stack. When we enter
*        an operand, it's pushed at the bottom of its stack. When
*        an operation is made, it takes place on the stack.
*        Operations are not stored anywhere and simply manipulate
*        the stack. An operation (e.g. +, SIN, etc.) always follows
*        its operand(s) and when it's complete the bottom of the
*        stack gets overwritten with its result.
*        Below is an example to give you some intuition:
*        `4 * (3 + 2)` or in RPN  `4 3 2 + *`; we do `3+2` first, 
*        then `4*5`:
*        @verbatim
*        4 3 2 + *
*          |     |    |     |    |  4  |   |     |   |     |
*        4 |     | 3  |  4  | 2  |  3  | + |  4  | * |     | <- Y
*          |  4  |    |  3  |    |  2  |   |  5  |   | 20  | <- X
*          +-----+    +-----+    +-----+   +-----+   +-----+ 
*        @endverbatim
*        An expression does not have a unique RPN translation, e.g. 
*         `4 * (3 + 2)` can also be evaluated as `3 2 + 4 *`.  
*        This relies on automatically lifting the stack after each 
*        calculation to make space for new operands. More details
*        on how it works in each method's documentation.
*
*        After you read each method's docs, you can refer to the
*        following python code for the general implementation of a
*        RPN calculator. The way the HP35 differentiates itself is
*        via its internals (stack of 4 elements, LASTX, general
*        registers etc.).
*
*        @include docs/rpn.py
*
*        Inherits from:
*        - IBackend; to implement its abstract methods
*        - Subject; to be an observable subject by the Observer class
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
        lastx_(other.lastx_),
        keypad_(other.keypad_),
        sto_regs_(other.sto_regs_),
        flags_(other.flags_) {}
    ~Backend() {}
    /** @brief Swaps values of registers X and Y. */
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
     *        Initially: `X = 6, Y = 6`
     *
     *        Type: `9 + 13`
     *        @verbatim
     *        (L means lift the stack, aka shift up)
     *        |     |     |     |     |     |      |     |
     *        |  6  |  9  |  6  |  +  |     |  13  |  15 | 
     *        |  6  |     |  9  |     |  15 |      |  13 |
     *        +-----+     +-----+     +-----+      +-----+
     *            L = false     L = true    L = false
     *        @endverbatim
     * @param num Decimal number to insert.
     */
    void Insert(double num) override;
    /**
     * @brief Circularly rotates the stack down, e.g.:
     *        @verbatim
     *           before:     after:    |
     *        T->   4          1       |   t ----------+  T <-+
     *        Z->   3          4       |   z -------+  +> Z   |
     *        Y->   2          3       |   y ----+  +---> Y   |
     *        X->   1          2       |   x     +------> X   |
     *                                 |   -------------------+
     *        @endverbatim
     */                                
    void Rdn() override;
    /**
     * @brief Emulate enter key; shift the stack up, discarding T
     *        register and then clone Y into X, e.g.:
     *        @verbatim
     *           before:     after:    |
     *        T->   4          3       |   t  +-----------> T
     *        Z->   3          2       |   z -+   +-------> Z
     *        Y->   2          1       |   y -----+    +--> Y
     *        X->   1          1       |   x ----------+--> X
     *        @endverbatim
     */
    void Enter() override;
    /**
    * @brief The LASTX register is a companion to the stack: it holds
    *        holds the number that was in the X–register before the
    *        last numeric function (e.g. `SQRT` or `+`) was executed.
    *        Pressing LASTX inserts the value of LASTX in the stack.
    *        It's used to either reuse a value or correct mistakes.
    *
    *        @include docs/backend_lastx.txt
    */
    void LastX() override;
    /**
     * @brief Emulate pressing a function key. Function keys operate
     *        either on the X register (1-operand key, e.g. LOG) or
     *        on the X and Y registers (2-operand keys, e.g. +).
     *        If an 1-op calculation write the result directly to the
     *        X register. 2-op calculations is entered, use reg. X, Y
     *        as inputs, write output to Y and shift down the stack. 
     *        Example:
     *
     *        @verbatim
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
     *        @endverbatim
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
     *        Example: `12 7 -` gives 5
     *                 `6 2 / COS` gives `COS(6/2)`
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
    /** @brief Set all registers (entire stack) to zero */
    void Cls() override;
    /** @brief Insert the value of PI to register X */
    void Pi() override;

    /**
     * @brief TODO
     */
    void Eex(std::optional<double> token) override;
    /**
    * @brief TODO
    *
    * @param idx
    */
    void Sto(std::string name) override;
    /**
    * @brief TODO
    *
    * @param idx
    */
    void Rcl(std::string name) override;
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
    /**
     * @brief Maps a register name as defined in Key namespace to
     *        and index of the `sto_regs_` array. This way, the user 
     *        can store or load from a general register given its name,
     *        e.g. "A" -> 0, "B" -> 1, etc.
     */
    std::unordered_map<std::string, std::size_t> gen_regs_name2idx;
    Flags flags_;
};


} /* namespace Rpn */

#endif /* BACKEND_HPP */
