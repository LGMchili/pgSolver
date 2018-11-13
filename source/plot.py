import matplotlib.pyplot as plt
with open('./output.txt', 'r') as content:
    data = list(content)
    t = data[0].split();
    line = data[1].split();
    name = line[0]
    curve = line[1:]
    t = [float(x) for x in t]
    val = [float(x) for x in curve]
    plt.plot(t, val)
    plt.show()
