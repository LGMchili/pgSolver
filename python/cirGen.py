
n = 100
total_num = 2 * n * (n - 1)
with open('tstCir.sp', 'w') as file:
    # horizontal
    for i in range(n):
        y = i * n + 1
        for j in range(n - 1):
            x = i * (n - 1) + j + 1
            comp = ''
            comp += 'r' + str(x) + ' ' # type
            comp += 'n' + str(y) + ' ' + 'n' + str(y + 1) + ' '# num
            comp += '0.25'# value
            y += 1
            file.write(comp + '\n')
    # vertical
    for i in range(n - 1):
        y = i * n + 1
        for j in range(n):
            x = i * n + j + 1 + n * (n - 1)
            comp = ''
            comp += 'r' + str(x) + ' ' # type
            comp += 'n' + str(y) + ' ' + 'n' + str(y + n) + ' '# num
            comp += '0.25'# value
            y += 1
            file.write(comp + '\n')
    # add voltage pad
    csrc = 'i0 ' + 'x_0 gnd ' + '-0.8'
    rpad = 'r_pad ' + 'x_0 gnd ' + '1.25'
    lpad = 'l_pad ' + 'x_0 n1 ' + '1e-6'
    cpad = 'c_pad ' + 'n' + str(y + n - 1) + ' gnd ' + '1e-5'
    load = 'i_load ' + 'n' + str(y + n - 1) + ' gnd ' + ' pwl ' \
            + '.35e-3 0 .4e-3 0.5 .45e-3 .3 .5e-3 .8 .58e-3 0'
    directive = '.tran 0 2e-3 0.001e-3'
    for comp in [csrc, rpad, lpad, cpad, load, directive]:
        file.write(comp + '\n')
