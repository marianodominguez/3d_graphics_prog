#!/usr/bin/env python3
import numpy as np

def load_model(filename):
    patches=[]
    vertices=[]
    f=open(filename)
    npatches=int(f.readline())
    if npatches!=32:
        raise Exception("Incorrect model format")
    for i in range(npatches):
        line=f.readline()
        patches.append([int(x) for x in line.split(',')])
    nvertices=int(f.readline())
    for i in range(nvertices):
        line=f.readline()
        vertices.append( np.array([float(p) for p in line.split(',')]) )

    return {"patches":patches,"vertices":vertices};

m=load_model("../models/teapot")
print(m['patches'])
print(m['vertices'])
