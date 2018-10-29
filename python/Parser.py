
def netlistParsing(self, filename):
    # parsing the input netlist
    with open(filename, 'r') as content:
        for line in content:
            line = line.rstrip()
            if not line or line.startswith('*'):
                continue
            line  = line.lower()
            if len(wl) < 4 and wl[0].startswith(('*', '#')):
                return
            elif wl[0].startswith('r'):
                addResistor(wl[0], wl[1], wl[2], wl[3])
            elif wl[0].startswith('c'):
                addCapacitor(wl[0], wl[1], wl[2], wl[3])
            elif wl[0].startswith('v'):
                addVoltageSource(wl[0], wl[1], wl[2], wl[3])
            elif wl[0].startswith('i'):
                addCurrentSource(wl[0], wl[1], wl[2], wl[3])

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
