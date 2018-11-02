import matplotlib.pyplot as plt
import numpy as np
import scipy.linalg as ssl
import timeit
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
        self._Ag = np.empty(())
        self._Ac = np.empty(())
        self._Al = np.empty(())
        self._Ai = np.empty(())
        self._G = np.empty(())
        self._L = np.empty(())
        self._C = np.empty(())
        self._Ii = np.empty(())
        self._Il = np.empty(())
        self._Directvie = [0, 10, 0.01, 100] # [start, end, delta, steps]
        self._LineSpace = []
        self._Result = np.empty(())
        self._Vdd = 1
        self.netlistParsing(netlist)
        self.buildAdjacentMatrix()

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
                elif line.startswith('.vdd'):
                    self._Vdd = float(words[1])
                else:
                    self.addComponent(line)

    def addDirective(self, line):
        words = line.split()
        start = float(words[1])
        end = float(words[2])
        delta = float(words[3])
        self._Directvie = [start, end, delta, int((end - start) / delta)]
        for i in range(self._Directvie[3]):
            self._LineSpace.append(start + i*delta)

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

    def addInductor(self, name, Np, Nn, val):
        val = float(val)
        assert val > 0
        ind = Component(name, Np, Nn, val)
        self._Inductors.append(ind)

    def addCurrentSource(self, name, Np, Nn, val):
        waveform = [-float(val) for i in range(self._Directvie[3])]
        dcsrc = Component(name, Np, Nn, waveform)
        self._CurrentSource.append(dcsrc)

    def addPwlCurrentSource(self, line):
        words = line.split()
        name = words[0]
        Np = words[1]
        Nn = words[2]
        waveform = []
        temp = words[4:] # time, value, time, value, ..., ...
        temp = [float(x) for x in temp]
        times = temp[0::2] # indexing by [start:end:step]
        values = temp[1::2]
        for i in range(len(times) - 1):
            length = int((times[i+1] - times[i]) / self._Directvie[2])
            k = (values[i+1] - values[i]) / length
            for j in range(length):
                waveform.append(-(values[i] + k * j))
        if(self._Directvie[3] - len(waveform) > 0):
            for j in range(self._Directvie[3] - len(waveform)):
                waveform.append(0)
        pwlsrc = Component(name, Np, Nn, waveform)
        self._CurrentSource.append(pwlsrc)

    def addVoltageSource(self, name, Np, Nn, val):
        val = float(val)
        vsrc = Component(name, Np, Nn, val)
        self._VoltageSource.append(vsrc)

    def addComponent(self, line):
        words = line.split()
        name = words[0]
        Np = words[1]
        Nn = words[2]
        if Np not in ['GND', 'gnd', '0'] and Np not in self._Nodes:
            n = len(self._NodeMap)
            self._NodeMap[Np] = n
            self._Nodes.append(Np)
            # print("add node %s as %d" %(Np,n))
        if Nn not in 'GNDgnd0' and Nn not in self._Nodes:
            n = len(self._NodeMap)
            self._NodeMap[Nn] = n
            self._Nodes.append(Nn)
            # print("add node %s as %d" %(Np,n))
        if len(words) == 4:
            val = words[3]
            if name.startswith('r'):
                self.addResistor(name, Np, Nn, val)
            elif name.startswith('c'):
                self.addCapacitor(name, Np, Nn, val)
            elif name.startswith('l'):
                self.addInductor(name, Np, Nn, val)
            elif name.startswith('v'):
                self.addVoltageSource(name, Np, Nn, val)
            elif name.startswith('i'):
                # dc source
                self.addCurrentSource(name, Np, Nn, val)
        elif(words[3] == 'pwl'):
            # pwl source
            self.addPwlCurrentSource(line)

    def buildAg(self):
        # size of adjacent matrix is decided by m = num of comps, n = num of nodes
        m = len(self._Resistors)
        n = len(self._NodeMap)
        self._Ag = np.zeros((m, n))
        diag = []
        for i in range(len(self._Resistors)):
            name = self._Resistors[i]._name
            Np = self._Resistors[i]._Np
            Nn = self._Resistors[i]._Nn
            val = 1 / self._Resistors[i]._val
            diag.append(val)
            if Np not in ['GND', 'gnd', '0']:
                self._Ag[i, self._NodeMap[Np]] = 1
            if Nn not in ['GND', 'gnd', '0']:
                self._Ag[i, self._NodeMap[Nn]] = -1
        self._G = np.diag(diag)

    def buildAc(self):
        m = len(self._Capacitors)
        n = len(self._NodeMap)
        self._Ac = np.zeros((m, n))
        diag = []
        for i in range(len(self._Capacitors)):
            name = self._Capacitors[i]._name
            Np = self._Capacitors[i]._Np
            Nn = self._Capacitors[i]._Nn
            val = self._Capacitors[i]._val
            diag.append(val)
            if Np not in ['GND', 'gnd', '0']:
                self._Ac[i, self._NodeMap[Np]] = 1
            if Nn not in ['GND', 'gnd', '0']:
                self._Ac[i, self._NodeMap[Nn]] = -1
        self._C = np.diag(diag)

    def buildAl(self):
        m = len(self._Inductors)
        n = len(self._NodeMap)
        self._Al = np.zeros((m, n))
        diag = []
        for i in range(len(self._Inductors)):
            name = self._Inductors[i]._name
            Np = self._Inductors[i]._Np
            Nn = self._Inductors[i]._Nn
            val = self._Directvie[2] / self._Inductors[i]._val # h / L
            diag.append(val)
            if Np not in ['GND', 'gnd', '0']:
                self._Al[i, self._NodeMap[Np]] = 1
            if Nn not in ['GND', 'gnd', '0']:
                self._Al[i, self._NodeMap[Nn]] = -1
            self
        self._L = np.diag(diag)
        self._Il = np.zeros((len(self._L), 1))
        self._Il[:] = self._Vdd

    def buildAi(self):
        m = len(self._CurrentSource)
        n = len(self._NodeMap)
        self._Ai = np.zeros((m, n))
        vectors = []
        for i in range(len(self._CurrentSource)):
            name = self._CurrentSource[i]._name
            Np = self._CurrentSource[i]._Np
            Nn = self._CurrentSource[i]._Nn
            val = self._CurrentSource[i]._val
            vectors.append(val)
            if Np not in ['GND', 'gnd', '0']:
                self._Ai[i, self._NodeMap[Np]] = 1
            if Nn not in ['GND', 'gnd', '0']:
                self._Ai[i, self._NodeMap[Nn]] = -1
        self._Ii = vectors

    def buildAdjacentMatrix(self):
        self.buildAg()
        self.buildAc()
        self.buildAl()
        self.buildAi()

    def buildCompMatrix(self, adj, diag):
        mat = np.dot(adj.T, diag)
        return np.dot(mat, adj)

    def buildCompVector(self, adj, vector):
        mat = np.dot(adj.T, vector)
        return mat

    def updateIndCurrent(self, currVolt):
        # TODO:
        for i in range(len(self._Il)):
            self._Il[i] = 1 - currVolt[self._NodeMap[self._Inductors[i]._Np]] / 1
        # print(self._Il)

    def solve(self):
        n = len(self._Nodes)# number of node
        h = self._Directvie[2]
        G = self.buildCompMatrix(self._Ag, self._G)
        C = self.buildCompMatrix(self._Ac, self._C)
        L = self.buildCompMatrix(self._Al, self._L)
        Ii = self.buildCompVector(self._Ai, self._Ii)
        # Il = self.buildCompVector(self._Al, self._Il)[:, 0]
        vn = np.ones((n, self._Directvie[3]))
        # solve the system -> Ax = b
        A = G + (C / h) + L
        mat_rhs = C / h
        lu, piv = ssl.lu_factor(A)
        for step in range(1, self._Directvie[3]):
            v = np.dot(mat_rhs, vn[:, step - 1]) # first part of the right hand side
            i = Ii[:, step] # second part of the right hand side
            self.updateIndCurrent(vn[:, step - 1])
            l = self.buildCompVector(self._Al, self._Il)[:, 0]
            rhs = v + i - l
            vn[:, step] = ssl.lu_solve((lu, piv), rhs)
            # vn[:, step] = np.linalg.solve(A, rhs)
            # print(vn[:, step])
            # break
        self._Result = vn

    def plot(self, node):
        waveform = self._Result[self._NodeMap[node], :]
        # print(waveform)
        plt.subplot(211)
        plt.plot(self._LineSpace, waveform)

    def plotCurrentSource(self, k):
        waveform = self._Ii[k]
        plt.subplot(212)
        plt.plot(self._LineSpace, waveform)

if __name__ == "__main__":
    simulator = Simulator('testCir.sp')
    start = timeit.default_timer()
    simulator.solve()
    stop = timeit.default_timer()
    print(str(stop - start) + 's')
    simulator.plot('n1_0_0')
    simulator.plotCurrentSource(1)
    plt.show()
