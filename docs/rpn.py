from math import sin, cos, tan

expr = "5 4 2 / SIN - 1 +" # i.e. 5 - sin(4/2) + 1
operands = [0]
lift = lambda x: x.insert(0, 0)
drop = lambda x: x.pop(0)
press_enter, do_lift = False, False
for token in expr.split(' '):
    if not token.lower() in ['+', '-', '*', '/', 'sin', 'cos', 'tan']:
        if do_lift or press_enter:
            lift(operands)
        if press_enter:
            operands[0] = operands[1]
        operands[0] = float(token)
        press_enter = True # press enter to separate the numbers
        do_lift = False
    else:
        if token.lower() in ['sin', 'cos', 'tan']: # one operand: X = f(X)
            operands[0] = eval(token.lower() + '(' + str(operands[0]) + ')')
        elif token in ['+', '-', '*', '/']: # two operands: X = f(X, Y)
            # store result to Y
            operands[1] = eval(str(operands[1]) + token + str(operands[0]))
            drop(operands) # drop X to make Y the new X
        press_enter, do_lift = False, True
