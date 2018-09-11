from Circuit import SpiceCircuit
import numpy as np
import scipy.io
from scipy.linalg import lu
class SpiceParser:
    """This class takes a filename or buffer in a Spice-like format
    and constructs a Circuit object.
    """

    def __init__(self, filename=None):
        self._fileName = filename
        self._circuit = SpiceCircuit()

        if self._fileName is not None:
            f = open(filename, 'r')
            self.ParseLines(f.readlines())
            f.close()

    def ParseLines(self, lines):
        for line in lines:
            self.ParseLine(line)

    def ParseLine(self, line):
        line = line.lower()
        wl = line.strip().split()
        if not wl:
            return
        if len(wl) < 4 and wl[0].startswith(('*', '#')):
            return
        elif wl[0].startswith('r'):
            self._circuit.addResistor(wl[0], wl[1], wl[2], wl[3])
        elif wl[0].startswith('c'):
            self._circuit.addCapacitor(wl[0], wl[1], wl[2], wl[3])
        elif wl[0].startswith('v'):
            self._circuit.addVoltageSource(wl[0], wl[1], wl[2], wl[3])
        elif wl[0].startswith('i'):
            self._circuit.addCurrentSource(wl[0], wl[1], wl[2], wl[3])

if __name__ == "__main__":
    parser = SpiceParser('../test/3x3.net')
    parser._circuit.createdmittanceMatrix()
    parser._circuit.createCurrentVector()
    parser._circuit.debugCircuit()
    # matrix = parser._circuit.getAdmittanceMatrix()

    # vector = parser._circuit.getCurrentVector()
    # scipy.io.savemat('./netlist.mat', mdict={'matrix': matrix})
    # np.save('./netlist', matrix)

    # res = np.linalg.solve(matrix,vector)
    # l = np.linalg.cholesky(matrix)
    # p, l, u = lu(matrix)
    # print(matrix)
    # print(vector)
    # print(res)
    # print(u)
    # print(np.transpose(l))
    # print(np.dot(l,u))
    # or l @ u
