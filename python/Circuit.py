import numpy as np

class Component(object):
    def __init__(self, type, Np, Nn, val):
        self._type = type
        self._Np = Np
        self._Nn = Nn
        self._val = val

class SpiceCircuit(object):
    """docstring for [object Object]."""

    def __init__(self):
        # super([object Object], self).__init__()
        self._NodeMap = {}
        self._Nodes = []
        self._Resistors = []
        self._Capacitors = []
        self._CurrentSource = []
        self._VoltageSource = []
        self._adjacencyMatrix = np.empty(())
        self._Vector = np.empty(())
        self._Ar = np.empty(())
        self._Ac = np.empty(())

    def mapNode(self, nodeName):
        if nodeName not in 'GNDgnd0':
            return 0
        if nodeName not in self.NodeMap:
            self.Size += 1
            self.NodeMap[nodeName] = self.Size
        return self.NodeMap[nodeName]

    def addResistor(self, name, Np, Nn, val):
        val = float(val)
        res = Component(name, Np, Nn, val)
        self._Resistors.append(res)
        assert val > 0
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

    def addCapacitor(self, name, Np, Nn, val):
        val = float(val)
        cap = Component(name, Np, Nn, val)
        self._Capacitors.append(cap)
        assert val > 0
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

    def addCurrentSource(self, name, Np, Nn, val):
        val = float(val)
        csrc = Component(name, Np, Nn, val)
        self._CurrentSource.append(csrc)
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

    def addVoltageSource(self, name, Np, Nn, val):
        val = float(val)
        vsrc = Component(name, Np, Nn, val)
        self._VoltageSource.append(vsrc)
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

    def getAdjacencyMatrix(self):
        m = len(self._Resistors)
        n = len(self._Nodes)
        self.matrix = np.zeros((m, n))
        index = 0
        for res in self._Resistors:
            Np = res._Np
            Nn = res._Nn
            val = res._val
            if(Np not in 'GNDgnd0'):
                self.matrix[index][self._NodeMap[Np]] = 1
            if(Nn not in 'GNDgnd0'):
                self.matrix[index][self._NodeMap[Nn]] = -1
            index += 1

        return self.matrix

    def createdmittanceMatrix(self):
        k = len(self._Nodes)
        m = len(self._Resistors)
        n = k + len(self._VoltageSource)
        print('size of netlist: %d' %n)
        self._Ar = np.zeros((n,n))
        self._Ac = np.zeros((n,n))
        for res in self._Resistors:
            Np = res._Np
            Nn = res._Nn
            val = res._val
            if Np not in 'GNDgnd0':
                self._Ar[self._NodeMap[Np]][self._NodeMap[Np]] += 1 / val
            if Nn not in 'GNDgnd0':
                self._Ar[self._NodeMap[Nn]][self._NodeMap[Nn]] += 1 / val
            if Nn not in 'GNDgnd0' and Np not in 'GNDgnd0':
                self._Ar[self._NodeMap[Np]][self._NodeMap[Nn]] -= 1 / val
                self._Ar[self._NodeMap[Nn]][self._NodeMap[Np]] -= 1 / val

        for cap in self._Capacitors:
            Np = cap._Np
            Nn = cap._Nn
            val = cap._val
            if Np not in 'GNDgnd0':
                self._Ac[self._NodeMap[Np]][self._NodeMap[Np]] += 1 / val
            if Nn not in 'GNDgnd0':
                self._Ac[self._NodeMap[Nn]][self._NodeMap[Nn]] += 1 / val
            if Nn not in 'GNDgnd0' and Np not in 'GNDgnd0':
                self._Ac[self._NodeMap[Np]][self._NodeMap[Nn]] -= 1 / val
                self._Ac[self._NodeMap[Nn]][self._NodeMap[Np]] -= 1 / val
        vindex = 0
        for vsrc in self._VoltageSource:
            Np = vsrc._Np
            Nn = vsrc._Nn
            val = vsrc._val
            if Np not in 'GNDgnd0':
                self._Ar[k + vindex][self._NodeMap[Np]] += val
                self._Ar[self._NodeMap[Np]][k + vindex] += val
            if Nn not in 'GNDgnd0':
                self._Ar[k + vindex][self._NodeMap[Np]] -= val
                self._Ar[self._NodeMap[Np]][k + vindex] -= val
            vindex += 1

    def createCurrentVector(self):
        k = len(self._Nodes)
        m = len(self._CurrentSource)
        n = k + len(self._VoltageSource)
        self._Vector = np.zeros((n,1))
        for isrc in self._CurrentSource:
            Np = isrc._Np
            Nn = isrc._Nn
            val = isrc._val
            if Np not in 'GNDgnd0':
                self._Vector[self._NodeMap[Np]] -= val
            if Nn not in 'GNDgnd0':
                self._Vector[self._NodeMap[Nn]] += val
        vindex = 0
        for vsrc in self._VoltageSource:
            Np = isrc._Np
            Nn = isrc._Nn
            val = isrc._val
            if Np not in 'GNDgnd0':
                self._Vector[vindex + k] += val
            if Nn not in 'GNDgnd0':
                self._Vector[vindex + k] -= val
            vindex += 1
    def debugCircuit(self):
        # print('Node map:')
        # print(self._NodeMap)
        # print('Number of nodes in netlist: %d' %(len(self._Nodes)))
        # print('Number of resistors: %d' %(len(self._Resistors)))
        # print('Number of capacitors: %d' %(len(self._Capacitors)))
        # print('Number of current source: %d' %(len(self._CurrentSource)))
        # print('Number of voltage source: %d' %(len(self._VoltageSource)))
        print(self._Ar)
        # print(self._Vector)
