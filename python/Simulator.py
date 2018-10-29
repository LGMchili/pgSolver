import numpy as np
class Component(object):
    def __init__(self, name, Np, Nn, val):
        self._name = name
        self._Np = Np
        self._Nn = Nn
        self._val = val

class Simulator(object):
    """docstring for Simulator."""
    def __init__(self, netlist):
        self._NodeMap = {} # node with number
        self._Nodes = []
        self._Resistors = []
        self._Capacitors = []
        self._Inductors = []
        self._CurrentSource = []
        self._VoltageSource = []
        self._adjacencyMatrix = np.empty(())
        self._Vector = np.empty(())
        self._Ar = np.empty(())
        self._Ac = np.empty(())
        self._Directvie = [0, 10, 0.01, 100] # [start, end, delta, steps]
        self.netlistParsing(netlist)

    def netlistParsing(self, filename):
        # parsing the input netlist
        with open(filename, 'r') as content:
            for line in content:
                line = line.rstrip()
                line  = line.lower()
                words = line.split()
                if not words or len(words) < 4 or words[0].startswith(('*', '#')):
                    continue
                if line.startswith('.tran'):
                    # .tran startTime endTime delta
                    self.addDirective(line)
                elif len(words) == 4:
                    self.addComponent(words[0], words[1], words[2], words[3])
                else:
                    pass
                    # pwl source

    def addDirective(self, line):
        words = line.split()
        start = float(words[1])
        end = float(words[2])
        delta = float(words[3])
        self._Directvie = [start, end, delta, int((end - start) / delta)]

    def addResistor(self, name, Np, Nn, val):
        val = float(val)
        assert val > 0
        res = Component(name, Np, Nn, val)
        self._Resistors.append(res)

    def addCapacitor(self, name, Np, Nn, val):
        val = float(val)
        assert val > 0
        cap = Component(name, Np, Nn, val)
        self._Capacitors.append(cap)

    def addCurrentSource(self, name, Np, Nn, val):
        val = float(val)
        csrc = Component(name, Np, Nn, val)
        self._CurrentSource.append(csrc)

    def addCurrentSourcePWL(self, wl):
        name = wl[0]
        Np = wl[1]
        Nn = wl[2]
        waveform = []
        pairs = []
        for i in range(2, len(wl)):
            pairs.append([wl[i-1], wl[i]])

    def addVoltageSource(self, name, Np, Nn, val):
        val = float(val)
        vsrc = Component(name, Np, Nn, val)
        self._VoltageSource.append(vsrc)

    def addComponent(self, name, Np, Nn, val):
        if Np not in 'GNDgnd0' and Np not in self._Nodes:
            n = len(self._NodeMap)
            self._NodeMap[Np] = n
            self._Nodes.append(Np)
            # print("add node %s as %d" %(Np,n))
        if Nn not in 'GNDgnd0' and Nn not in self._Nodes:
            n = len(self._NodeMap)
            self._NodeMap[Nn] = n
            self._Nodes.append(Nn)
            # print("add node %s as %d" %(Np,n))
        if name.startswith('r'):
            self.addResistor(name, Np, Nn, val)
        elif name.startswith('c'):
            self.addCapacitor(name, Np, Nn, val)
        elif name.startswith('v'):
            self.addVoltageSource(name, Np, Nn, val)
        elif name.startswith('i'):
            self.addCurrentSource(name, Np, Nn, val)

if __name__ == "__main__":
    simulator = Simulator('testCir.sp')
    print(simulator._Directvie)
