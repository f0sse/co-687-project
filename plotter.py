##
## plotter.py
## lucas@pamorana.net
##
## For a project in CO 687, UW, Fall 2024.
## Create a graph of a list of coordinates given on STDIN.
##

import sys

from matplotlib import pyplot as plt

y = []

bound = 0.0

if not sys.stdin.closed:
	#
	for line in map(str.strip, sys.stdin):
		if "bound:" in line:
			bound = float(line.split(":")[-1])
		if line == "BEGIN COORDINATES":
			break
		print(line)
	#
	for line in map(str.strip, sys.stdin):
		if line == "END COORDINATES":
			break
		index, cputime = map(float, line.split(","))
		y.append(cputime)
	#
	for line in map(str.strip, sys.stdin):
		print(line)
	#
	if bound == 0.0:
		# if each bit is fair, this should provide a good enough estimate
		bound = sum(y[1:]) / (len(y) - 1)

f=plt.figure(1)
x=list(range(1, len(y)+1))
plt.plot(x, y, '.b', label=None)
plt.plot(1, y[0], '.b', label="Time")
plt.axhline(bound, linestyle='--', color='k', label='Boundary', linewidth=.5)
plt.xlabel("Bit number")
plt.ylabel("CPU time $[s]$")
plt.legend()
plt.title("Time per bit in $k$")
plt.show(block=True)
f.savefig("optime.pdf", bbox_inches='tight')
