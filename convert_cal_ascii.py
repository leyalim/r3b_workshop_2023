#!/usr/bin/python
from __future__ import print_function

import  ctypes, sys, operator, code, math
import functools, itertools, time, re, os, json
from collections import deque

calibration={}

infile=open("calib_params_final.txt")

for line in infile:
    sl=(line.split("\t"))
    crId=int(sl[0])
    offset=float(sl[1])
    slope=float(sl[2])
    print(crId, slope, offset)
    calibration[crId]=(slope, offset, "egal")



out=open("cal_PuC.par", "w")
# we try to output everything just as FairROOT would, so
# it will have less of a chance to complain. 

def write_hashes():
    out.write(78*"#"+"\n")

def write_header(classname, context):
    write_hashes()
    out.write("# Class:   %s\n"%classname)
    out.write("# Context: %s\n"%context)
    write_hashes()
    out.write("[%s]\n"%classname)
    out.write("//"+76*"-"+"\n")
#bad stuff will happen if this is not present
write_header("FairBaseParSet", "DefaultContext")
out.write("Beam Momentum Gev/c:  Double_t  15 \n")
out.write("Random Seed:  UInt_t  ") # NO value or \n here
write_hashes()
write_header("FairGeoParSet", "DefaultContext")
write_hashes()

write_header("califaCrystalCalPar", "CalifaCalParContext")
out.write("califaCrystalCalPar:  Float_t")
nCrystals=2432*2
print("Exporting calibration for %d crystals."%nCrystals)

initial=float('nan')
pars=map(lambda x:initial, range(nCrystals*2))

for (crId, (m, c, ratio)) in calibration.items():
    crId=int(crId)
    pars[2*(crId-1)]=c
    pars[2*(crId-1)+1]=m
pars=deque(pars)
while(pars):
    out.write(" \\\n ")
    for i in range(min(10, len(pars)) ):
        out.write(" %f"%pars.popleft())
out.write("\n")
out.write("califaCrystalNumberPar:  Int_t  %d \n"%nCrystals)
out.write("califaCrystalParamsFitPar:  Int_t  %d \n" % 2)
write_hashes()
#print(rtdb.writeContainer(calClass, run))
