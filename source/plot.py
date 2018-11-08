import matplotlib.pyplot as plt
with open('./output.txt', 'r') as content:
    data = list(content)
    t = data[0].split();
    val = data[1].split();
    t = [float(x) for x in t]
    val = [float(x) for x in val]
    plt.plot(t, val)
    plt.show()
