# HIP35

## Introduction

### What's this project?
HIP-35 is a calculator for hipsters. It emulates the RPN (reverse
Polish notation) mode of HP-35 from 1972. It runs a terminal UI
built with ncurses that displays the current result on a display. 

### The memory
HP-35 had a stack-based memory of 4 registers `X`, `Y`, `Z`, `T`
with `X` being the bottom (entry). That stack shifts (lifts or
drops) automatically as calculations are performed. There are
also 10 general registers labelled `A` to `J` that store values,
acting as buffers.

| HP-35 | HIP-35 |
|---------|---------|
| <img src="image1.jpg" alt="Alt text 1" width="200"/> | <img src="image2.jpg" alt="Alt text 2" width="300"/> |

## Usage

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

It works like any RPN calculator however enter (`<space>`) needs to 
be pressed to separated two successive numbers. When running the UI,
press `q` to quit. `<Ctr-C>` is not captured so the only way to quit.

A unit test executable is also generated at
`./build/test/testhip35`.


## Basic theory 
### What is RPN?

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
| 1 + 2 + 3  | 1 2 3 + + or: | 1 E 2 E 3 + +     | ...[1,2], [1,2,2], [1,2,3], [1,5], [6]        |
|            | 1 2 + 3 +     | 1 E 2 + 3 +       | ...[1,2], [3], [3,3], [6]                     |
+------------+---------------+-------------------+-----------------------------------------------+
| (1+2)(3+4) | 1 2 + 3 4 + * | 1 E 2 + 3 E 4 + * | ...[1,2], [3], [3,3], [3,3,4], [3,7], [21]    |
+------------+---------------+-------------------+-----------------------------------------------+
| 13*sin(37) | 13 37 SIN *   | 13 E 37 SIN *     | [13], [13,13], [13,37] [13,s(37)], [13*s(37)] |
+------------+---------------+-------------------+-----------------------------------------------+
```
You can practise more with RPN by using the script `docs/rpn.py` 
which evaluates RPN expressions from a string. Numbers are
separated by space in this script so you don't need to press enter.

### Why RPN?

RPN makes it very easy to compute long expressions containing
lots of brackets [1]. When using it, you don't need to think how
terms are grouped anymore. With a little practice it's faster
and it saves keystrokes to evaluate in RPN.

### HIP-35 additional features
HIP-35 includes some minor additions compared to HP-35:
* The display now has two rows, displaying the X and Y registers
instead of just X.
* There is space on the right to display the 10 general registers
- back in the day users had to write down which register stores
what.
* User can also enter negative numbers using the `~` as the unary
minus. HP35 only supposed the `CHS` (change sign) operation for
that.
* There is no `ARC` key. `ARC` key had to pressed to enable the
`SIN, COS, TAN` keys to compute `ASIN, ACOS, ATAN`. The latter
keys are directly implemented in HIP-35.
* The number format on the display is automatically adjusted based 
on the range the number is in.
* The LASTX key from HP35s is ported as it makes calculations
much easier.

You can read more at the [wiki](https://en.wikipedia.org/wiki/HP-35)
page, at [HP museum](https://www.hpmuseum.org/hp35.htm) or at the
[HP35s manual](http://h10032.www1.hp.com/ctg/Manual/c01579350), 
which is its successor.

### Cool trivia

* HP35 was the first ever scientific pocket calculator. It was
  released in 1972. Before that, the only practical portable
  devices for performing trigonometric and exponential functions
  were slide rules [2].
* The co-founder of HP himself, Bill Hewlett, challenged his
coworkers to create it as a "shirt-pocket sized HP-9100“ (HP-9100
was a scientific computer from the 60s).
* HP-35 calculators, were used by NASA's Skylab crew from 1973 to
1974 [3]. It's rumored to be the first calculator to ever fly into
space. It helped the crew write programs that perform calculations
related to the spacecraft's orbit around the moon (Lunar Orbital
Rendezvous or LOR).
* It was able to perform calculations both in algebraic a RPN mode.

## References

[1] https://leachlegacy.ece.gatech.edu/revpol/
[2] https://killerinnovations.com/interview-with-david-cochran-on-creating-the-hp35-calculator/  
[3] https://core.ac.uk/download/pdf/10564548.pdf 
