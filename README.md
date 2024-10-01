# socialmarkets

This is a toy project exploring an analogy between market forces and spring physics.

Each node represents some resource, and an edge represents a market in which one resource is exchanged for another.

The desired length of the spring is related to the equilibrium price between the two.

This follows the expected behavior of a damped oscillator, which means price elasticity can be adjusted with the 'spring coefficient' of the graph edge.

### todo:

- Allow equillibrium prices to be affected by other nodes somehow.

- Use A* in an amusing manner to possibly perform some form of market arbitrage.

- Somehow relate this to real markets by accepting initial states, running the sim and making a prediction.

- Come up with some excuse for the dimensionality of the simulation.

(Currently the simulation works in any dimensional euclidian space (adjust `bnum`), but obviously only displays the first 2 dims)


