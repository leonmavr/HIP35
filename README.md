```
 .----------------. .----------------. .----------------. .----------------. .----------------. .----------------. 
| .--------------. | .--------------. | .--------------. | .--------------. | .--------------. | .--------------. |
| |  ____  ____  | | |     _____    | | |   ______     | | |              | | |    ______    | | |   _______    | |
| | |_   ||   _| | | |    |_   _|   | | |  |_   __ \   | | |              | | |   / ____ `.  | | |  |  _____|   | |
| |   | |__| |   | | |      | |     | | |    | |__) |  | | |    ______    | | |   `'  __) |  | | |  | |____     | |
| |   |  __  |   | | |      | |     | | |    |  ___/   | | |   |______|   | | |   _  |__ '.  | | |  '_.____''.  | |
| |  _| |  | |_  | | |     _| |_    | | |   _| |_      | | |              | | |  | \____) |  | | |  | \____) |  | |
| | |____||____| | | |    |_____|   | | |  |_____|     | | |              | | |   \______.'  | | |   \______.'  | |
| |              | | |              | | |              | | |              | | |              | | |              | |
| '--------------' | '--------------' | '--------------' | '--------------' | '--------------' | '--------------' |
 '----------------' '----------------' '----------------' '----------------' '----------------' '----------------'
```
<sub>(ASCII art by [patorjk.com](https://patorjk.com/software/taag))</sub>

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C/C++ CI build & test](https://github.com/leonmavr/HIP35/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/leonmavr/HIP35/actions/workflows/c-cpp.yml)

## 1. Introduction

### What's this project?
HIP-35 is a calculator for hipsters. It emulates the RPN (reverse
Polish notation) mode of HP-35 from 1972. It runs a terminal UI
built with ncurses that displays the current result on a display.

### ⚫ Why RPN?

Because it genally involves less keystrokes and it's faster than
algebraic notation calculations.  

How? RPN makes it very easy to compute long expressions containing
lots of brackets [[1]](#ref-1). When using it, you don't need
to think how terms are grouped anymore.

### ⚫ The memory
HP-35 had a stack-based memory of 4 registers `X`, `Y`, `Z`, `T`
with `X` being the bottom (entry). That stack shifts (lifts or
drops) automatically as calculations are performed. There are
also 10 general registers labelled `A` to `J` that store values,
acting as buffers.

| HP-35 | HIP-35 |
|---------|---------|
| <img src="https://raw.githubusercontent.com/leonmavr/HIP35/master/assets/wiki_image.jpg" alt="HP 35 front face" height="325"/> | <img src="https://raw.githubusercontent.com/leonmavr/HIP35/master/assets/demo_screenshot.png" alt="hip 35 screenshot" height="325"/> |

## 2. Usage

### ⚫ Dependencies

CMake is required to build the project and ncurses for the frontend.
Also you need to make sure your compuler supports **C++17** or higher.
Install both packages by:

| Arch-based                     | Debian-based                                       |
|--------------------------------|----------------------------------------------------|
| `sudo pacman -S cmake ncurses` | `sudo apt install cmake libncurses5 libncursesw5`  |

### ⚫ Compilation
The build system is based on CMake. To build the project run:
```
cmake . -Bbuild
cd build
make -j
```
or alternatively just execute the `build.sh` script:
```
./build.sh
```
The demo executable that runs the UI will be generated at
```
./build/demo/demo
```
That's it, have fun doing RPN calculations!

A unit test executable is also generated at
`./build/test/testhip35`.

### ⚫ The keys

Most keys are self-explanatory. However, some are less straightforward.

| key   | description             | syntax    | before            | argument | after             |
|-------|-------------------------|-----------|-------------------|----------|-------------------|
| CHS   | change sign of X        | `CHS`     | 4                 |          | -4                |
| ENTER | duplicate X to Y        | `ENTER`   | `T,Z,Y,X=1,2,3,4` |          | `T,Z,Y,X=2,3,4,4` |
| LASTX | last X before operation | `LASTX`   | `4 E 20 +` (X=24) |          | `X=20`            |
| RDN   | rotate stack down       | `RDN`     | `T,Z,Y,X=1,2,3,4` |          | `T,Z,Y,X=4,3,2,1` |
| STO   | store to A-J e.g. A = X | `STO a-j` | `X=3.14, A=0`     | a        | `X=3.14, A=3.14`  |
| RCL   | recall to X e.g. X = A  | `RCL a-j` | `X=0, F=2.71`     | f        | `X=2.71, F=2.71`  |
| EEX   | exponentiation of X     | `EEX N`   | 7                 | 3        | 7000              |
| CLX   | clear register X        | `CLX`     | 12345             |          | 0                 |
| CLR   | clear entire stack      | `CLR`     | `T,Z,Y,X=1,2,3,4` |          | `T,Z,Y,X=0,0,0,0` |
| q     | quit application        | `q`       |                   |          |                   |



Enter (`<space>`) needs to be pressed to separate two successive
numbers. When running the UI, press `q` to quit. `<Ctr-C>` is 
not captured so `q` is the only way to quit. You can read more 
at the [HP35 manual](https://literature.hpcalc.org/community/hp35-om-en-reddot.pdf)
[[4]](#ref-4).

### ⚫ Project directory structure and source code

Implementations are found at `lib/src` and header files at `lib/inc`. The demo application is at `demo/main.cpp` and unit tests are found at `test/test.cpp`. 

You can create an instance of the Hip35 class and run it either via 
the UI or without it (given a string) as follows:
```
#include "hip35.h"
//...
auto hp = std::make_unique<Ui::Hip35>(key::keypad);
// using the UI
hp->RunUI();
// or without it
auto result = hp->EvalString("430 ENTER 80 - 1.2 *");
```

## 3. Demo

Second order equation by using storage/recall:

https://github.com/leonmavr/HIP35/assets/13514096/911e7cce-2db0-4705-92fd-c3dfa6142de6

The fraction `sqrt(2 + (sin(25) + pi/2)^2)/3`:

https://github.com/leonmavr/HIP35/assets/13514096/ee05b992-8901-4ffe-9649-71dfb9cda71c


## 4. Basic theory
### ⚫ What is RPN?

RPN allows a stack-based evaluation of numerical expressions,
eliminating the need of brackets. There are two main ideas behind
RPN:
1. operands (numbers) are pushed in a stack
2. it follows the postfix notation, i.e. each operator follows one
or two operands preceeding it

Physical calculators need a way to separate successive numbers,
accomplished with the enter (E) key.

For example:
```
+------------+---------------+-------------------+-----------------------------------------------+
| Algebrainc | RPN           | Physical          | Stack contents                                |
| expression | equivalent    | calculator        |                                               |
+------------+---------------+-------------------+-----------------------------------------------+
| 1 + 2      | 1 2 +         | 1 E 2 +           | [1], [1,1], [1,2], [3]                        |
+------------+---------------+-------------------+-----------------------------------------------+
| 1 + 2 - 3  | 1 2 3 + - or: | 1 E 2 E 3 + -     | ...[1,2], [1,2,2], [1,2,3], [1,-1], [0]       |
|            | 1 2 + 3 -     | 1 E 2 + 3 -       | ...[1,2], [3], [3,-3], [0]                    |
+------------+---------------+-------------------+-----------------------------------------------+
| (1+2)(3+4) | 1 2 + 3 4 + * | 1 E 2 + 3 E 4 + * | ...[1,2], [3], [3,3], [3,3,4], [3,7], [21]    |
+------------+---------------+-------------------+-----------------------------------------------+
| 13*sin(37) | 13 37 SIN *   | 13 E 37 SIN *     | [13], [13,13], [13,37] [13,s(37)], [13*s(37)] |
+------------+---------------+-------------------+-----------------------------------------------+
```
You can practise more with RPN by using the script `docs/rpn.py`
which evaluates RPN expressions from a string. Numbers are
separated by space in this script so you don't need to press enter.


### ⚫ HIP-35 additional features
HIP-35 includes some minor additions compared to HP-35:
* The display now has two rows, displaying the X and Y registers
instead of just X.
* There is space on the right to display the 10 general registers.
Back in the day users had to write down which register stores
what.
* User can also enter negative numbers using the `~` as the unary
minus. HP35 only supposed the `CHS` (change sign) operation for
that.
* There is no `ARC` key. `ARC` key had to pressed to enable the
`SIN, COS, TAN` keys to compute `ASIN, ACOS, ATAN`. The latter
keys are directly implemented in HIP-35.
* The number format on the display is automatically adjusted based
on the range the number is in.
* The `LASTX` key from HP-35s is ported as it makes certain 
calculations much faster.

You can read more at the [wiki](https://en.wikipedia.org/wiki/HP-35)
page, at [HP museum](https://www.hpmuseum.org/hp35.htm) or at the
[HP-35s manual](http://h10032.www1.hp.com/ctg/Manual/c01579350),
which is its successor.

### ⚫ Cool trivia

* HP35 was the first ever scientific pocket calculator. It was
  released in 1972. Before that, the only practical portable
  devices for performing trigonometric and exponential functions
  were slide rules [[2]](#ref-2)
* The co-founder of HP himself, Bill Hewlett, challenged his
coworkers to create it as a "shirt-pocket sized HP-9100“ (HP-9100
was a scientific computer from the 60s).
* HP-35 calculators, were used by NASA's Skylab crew from 1973 to
1974 [[3]](#ref-3). It's rumored to be the first
calculator to ever fly into space. It helped the crew write
programs that perform calculations related to the spacecraft's
orbit around the moon (Lunar Orbital Rendezvous or LOR).
* It was able to perform calculations both in algebraic a RPN mode.

## 5. Future ideas

- [ ] Paste text into display
- [ ] In headless mode (parsing a string), throw an error instead of
      continuing when a token is unknown.
- [ ] (ambitious) write a compiler for the HP35/HP35s programming
	  language and be able to write routines by pressing a key.

## 6. Donations

I'm looking to raise about 150-200 euros to buy a second hand
HP-35 calculator so I can compare my emulator against it. Feel
free to donate any amount to:


<img src="https://www.paypalobjects.com/webstatic/mktg/logo-center/PP_Acceptance_Marks_for_LogoCenter_266x142.png" alt="paypal link" height="30"/> [leonmavr](https://paypal.me/leonmavr)  
<img src="https://upload.wikimedia.org/wikipedia/commons/4/46/Bitcoin.svg" alt="bitcoin link" height="30"/> ``36e6wNQ64dGXGDKQMn8LzWC3zSon1SvVJQ``

## References

[1] <a name="ref-1"></a>[Dr Marshall Leach Jr's page](https://leachlegacy.ece.gatech.edu/revpol/)  
[2] <a name="ref-2">[Interview with David Cochran](https://killerinnovations.com/interview-with-david-cochran-on-creating-the-hp35-calculator/)  
[3] <a name="ref-3"></a>[History of Space Shuttle Rendezvous, Oct 2011](https://core.ac.uk/download/pdf/10564548.pdf)  
[4] <a name="ref-4"></a>[HP35 manual](https://literature.hpcalc.org/community/hp35-om-en-reddot.pdf)  

