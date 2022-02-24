# Improvements

This document follows all the possible improvements that were avoided at the time of implementation, or found later on. The reason for avoiding the implementation of the improvements is that they were time-consuming to design and implement and did not have a clear indicator whether they would add significant improvements overall.

## Scanner

### Regex tree

- The regex tree is used to build an operation precedence graph. The tree is currently implemented as a linked list with nodes allocated separately. This could be transformed into a single 1D array where the nodes could link eachother inside the array, giving improvements if the array stays relatively small.

### Finite automaton (fa.h, fa.cpp)