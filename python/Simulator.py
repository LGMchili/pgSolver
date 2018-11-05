import matplotlib.pyplot as plt
import numpy as np
import scipy.linalg as ssl
import timeit
import sys
from Parser import *

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
        self._A = np.empty(())
        self._B = np.empty(())
        self._Directvie = [0, 10, 0.01, 100] # [start, end, delta, steps]
        self._LineSpace = []
        self._Result = np.empty(())
        self._Vdd = 1
        self._parser = None
        self._filename = netlist
        self._parser = SpiceParser(self._filename)
        # self.buildAdjacentMatrix()

    def simulate(self):
        self.solve()

    def buildCircuit(self):
        Ag, diagG = self.buildAg()
        Ac, diagC = self.buildAc()
        Al, diagL = self.buildAl()
        Ai, vecI = self.buildAi()
        G = self.createMatrix(Ag, diagG)
        C = self.createMatrix(Ac, diagC)
        L = self.createMatrix(Al, diagL)
        Ii = self.createVector(Ai, vecI)
        h = self._parser._delta
        # solving Av(t+delta) = Bv(t) + I
        A = G + (2 * C / h) + (h * L / 2)
        B = -G + (2 * C / h) - (h * L / 2)
        S = h * L
        return A, B, S, Ii

    def solve(self):
        n = len(self._parser._Nodes) # number of node
        t = len(self._parser._steps) # number of steps
        vn = np.ones((n, len(self._parser._steps)))
        # solving Av(t+delta) = Bv(t) + I
        A, B, S, Ii = self.buildCircuit()
        lu, piv = ssl.lu_factor(A)
        il = np.zeros((n, ))
        # print(Ii.shape)
        # sys.exit()
        for step in range(1, t):
            iv = np.dot(B, vn[:, step - 1]) # first part of the right hand side
            ii = Ii[:, step] + Ii[:, step - 1] # second part of the right hand side
            # self.updateIndCurrent(vn[:, step - 1])
            # l = self.buildCompVector(self._Al, self._Il)[:, 0]
            rhs = iv + ii - il
            vn[:, step] = ssl.lu_solve((lu, piv), rhs)
            il = il + np.dot(S, vn[:, step] + vn[:, step - 1])
        self._Result = vn

    def buildNodeMap(self):
        pass

    def buildAg(self):
        # size of adjacent matrix is decided by m = num of comps, n = num of nodes
        m = len(self._parser._Resistors)
        n = len(self._parser._NodeMap)
        Ag = np.zeros((m, n))
        diag = []
        index = 0
        for res in self._parser._Resistors:
            name = res._name
            Np = res._Np
            Nn = res._Nn
            val = 1 / res._val
            diag.append(val)
            if Np not in ['GND', 'gnd', '0']:
                Ag[index, self._parser._NodeMap[Np]] = 1
            if Nn not in ['GND', 'gnd', '0']:
                Ag[index, self._parser._NodeMap[Nn]] = -1
            index += 1
        G = np.diag(diag)
        return Ag, G

    def buildAc(self):
        m = len(self._parser._Capacitors)
        n = len(self._parser._NodeMap)
        Ac = np.zeros((m, n))
        diag = []
        index = 0
        for cap in self._parser._Capacitors:
            name = cap._name
            Np = cap._Np
            Nn = cap._Nn
            val = cap._val
            diag.append(val)
            if Np not in ['GND', 'gnd', '0']:
                Ac[index, self._parser._NodeMap[Np]] = 1
            if Nn not in ['GND', 'gnd', '0']:
                Ac[index, self._parser._NodeMap[Nn]] = -1
            index += 1
        C = np.diag(diag)
        return Ac, C

    def buildAl(self):
        m = len(self._parser._Inductors)
        n = len(self._parser._NodeMap)
        Al = np.zeros((m, n))
        diag = []
        index = 0
        for ind in self._parser._Inductors:
            name = ind._name
            Np = ind._Np
            Nn = ind._Nn
            val = 1 / ind._val # h / L
            diag.append(val)
            if Np not in ['GND', 'gnd', '0']:
                Al[index, self._parser._NodeMap[Np]] = 1
            if Nn not in ['GND', 'gnd', '0']:
                Al[index, self._parser._NodeMap[Nn]] = -1
            index += 1
        L = np.diag(diag)
        return Al, L

    def buildAi(self):
        m = len(self._parser._CurrentSource)
        n = len(self._parser._NodeMap)
        Ai = np.zeros((m, n))
        Ii = []
        index = 0
        for csrc in self._parser._CurrentSource:
            name = csrc._name
            Np = csrc._Np
            Nn = csrc._Nn
            val = csrc._val
            Ii.append(val)
            if Np not in ['GND', 'gnd', '0']:
                Ai[index, self._parser._NodeMap[Np]] = 1
            if Nn not in ['GND', 'gnd', '0']:
                Ai[index, self._parser._NodeMap[Nn]] = -1
            index += 1
        self._Ii = Ii
        return Ai, Ii

    def createMatrix(self, adj, diag):
        mat = np.dot(adj.T, diag)
        return np.dot(mat, adj)

    def createVector(self, adj, vector):
        mat = np.dot(adj.T, vector)
        return mat

    def updateIndCurrent(self, currVolt):
        # TODO:
        for i in range(len(self._Il)):
            self._Il[i] = 1 - currVolt[self._NodeMap[self._Inductors[i]._Np]] / 1
        # print(self._Il)

    def plot(self, node):
        waveform = self._Result[self._parser._NodeMap[node], :]
        plt.subplot(211)
        plt.plot(self._parser._steps, waveform)

    def plotCurrentSource(self, k):
        waveform = self._Ii[k]
        plt.subplot(212)
        plt.plot(self._parser._steps, waveform)

if __name__ == "__main__":
    simulator = Simulator('testCir.sp')
    start = timeit.default_timer()
    simulator.simulate()
    stop = timeit.default_timer()
    print(str(stop - start) + 's')
    simulator.plot('n1_50_0')
    simulator.plotCurrentSource(1)
    plt.show()
