from math import sin, cos, tan

expr = "5 4 2 / -"
operands = [0]

press_enter = False 
for token in expr.split(' '):
    if not token.lower() in ['+', '-', '*', '/', 'sin', 'cos', 'tan']:
        if press_enter:
            operands.insert(0, operands[0])
        operands[0] = float(token)
        press_enter = True
    else:
        press_enter = False
        # one operand

        regx = operands[0]
        operands[0] = eval(token + '(' + str(operands[0])+ ')')
        # two operands
print(operands)
