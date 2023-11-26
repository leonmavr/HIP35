nums = []

# 1 + 2 + 5 * sin(3/2.1)
tokens = "1 2 + 5 3 2.1 / sin * +"

lift = False
drop = False
for token in tokens:
    if not token in ['+', '-', '*', '/', 'sin', 'cos', 'tan']:
        if not lift:
            nums[0] = float(token))
            lift = True
        else:
            nums.insert(0, float(token)
        

