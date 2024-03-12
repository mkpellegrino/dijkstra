# dijkstra
An implementation of Dijkstra's algorithm

Create nodes with unique coordinates.  These coordinates are using in the operator== overload, so they must be unique.
Link nodes together (automatically creates bi-directional links)
getEasiestPath( node * a, node * b ) will create the shortest path from a to b using the algorithm

Keep in mind that I wrote this to be part of a RISK strategy game AI

In the traditional Dijkstra algorithm, the paths between vertices have weights.  In _this_ implementation, the vertices (Nodes) have weights.
It's meant to represent the number of troops in an adversary's territory in risk.


A Node Map might look something like this:

(A5) -- (B1) -- (C7) -- (D9)
   \___  |     /  |  
       (E6)--(F1) |
                \ |
                 (G4)
