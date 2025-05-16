# Distance Vector Routing Simulation

Simulates Distance Vector Routing (DVR) using the Bellman-Ford algorithm across 5 routers in a multithreaded environment. Random delays and topology changes emulate real-world network behavior.

## Features
- Bellman-Ford algorithm for shortest path calculation
- Multithreaded router simulation (`pthreads`)
- Dynamic cost updates and delays
- Thread-safe with mutexes

## Build & Run
```bash
gcc -pthread -o dvr_sim bellman.c
./dvr_sim
