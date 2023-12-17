//Simiulation Distance Vector Routing (DVR) Protocol, centered around the Bellman-Ford algorithm 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

#define MAX_ROUTERS 5
#define INFINITY 9999
//simulates a vector-router network
//adds in a random delay + random update of edges to simulate real life changes

struct DistanceVector {
    int cost[MAX_ROUTERS];
};

struct DistanceVector routers[MAX_ROUTERS];
pthread_mutex_t lock[MAX_ROUTERS]; // mutex locks for each router
bool vectorChanged[MAX_ROUTERS]; // indicates if a router's vector changed

void initializeNetwork() {
    // initialize all costs between routers
    for (int i = 0; i < MAX_ROUTERS; i++) {
        for (int j = 0; j < MAX_ROUTERS; j++) {
            if (i == j) {
                routers[i].cost[j] = 0; // self-distance is 0
            } else {
                routers[i].cost[j] = INFINITY; // other distances set to infinity
            }
        }
    }
    
    //arbitrary initial costs between routers
    routers[0].cost[1] = 1;
    routers[0].cost[2] = 5;
    routers[1].cost[3] = 3;
    routers[2].cost[3] = 1;
    routers[3].cost[4] = 2;
}

void displayDistanceVectors() {
    //display current distance vectors for each router
    printf("Router Distance Vectors:\n");
    for (int i = 0; i < MAX_ROUTERS; ++i) {
        printf("Router %d: ", i);
        for (int j = 0; j < MAX_ROUTERS; ++j) {
            if (routers[i].cost[j] == INFINITY) {
                printf("INF ");
            } else {
                printf("%d ", routers[i].cost[j]);
            }
        }
        printf("\n");
    }
}

//function executed by each router thread
void *simulateDVAlgorithm(void *routerIDPtr) {
    int routerID = *((int *)routerIDPtr); // Router ID
    
    printf("Router %d starts recalculating its distance vector...\n", routerID);

    do {
        vectorChanged[routerID] = false; // 4eset change flag for this router

        pthread_mutex_lock(&lock[routerID]); // lock access to this router's data
        for (int dest = 0; dest < MAX_ROUTERS; ++dest) {
            int minCost = routers[routerID].cost[dest]; // minimum cost to destination initially set to current cost

            for (int neighbor = 0; neighbor < MAX_ROUTERS; ++neighbor) {
                int costToNeighbor = routers[routerID].cost[neighbor];
                if (costToNeighbor != 0 && costToNeighbor != INFINITY) {
                    // calculate total cost via neighbors
                    int totalCost = costToNeighbor + routers[neighbor].cost[dest];
                    if (totalCost < minCost) {
                        minCost = totalCost; // update minimum cost if a shorter path is found
                    }
                }
            }

            //update the router's cost to the destination if a shorter path is found
            if (minCost < routers[routerID].cost[dest]) {
                routers[routerID].cost[dest] = minCost;
                vectorChanged[routerID] = true; // flag that the vector has changed
            }
        }
        pthread_mutex_unlock(&lock[routerID]); // unlock access to this router's data

        displayDistanceVectors(); // display current distance vectors

        //simulate random changes in the network topology by altering costs between routers
        if (rand() % 10 == 0) {
            int router1 = rand() % MAX_ROUTERS;
            int router2 = rand() % MAX_ROUTERS;
            int newCost = rand() % 10 + 1; // random cost between 1 to 10
            printf("Changing cost between Router %d and Router %d to %d\n", router1, router2, newCost);

            pthread_mutex_lock(&lock[router1]); // lock access to router1's data
            pthread_mutex_lock(&lock[router2]); // lock access to router2's data
            routers[router1].cost[router2] = newCost;
            routers[router2].cost[router1] = newCost;
            pthread_mutex_unlock(&lock[router2]); // unlock access to router2's data
            pthread_mutex_unlock(&lock[router1]); // unlock access to router1's data

            vectorChanged[router1] = true; // flag that the vectors of router1 and router2 have changed
            vectorChanged[router2] = true;
        }

        int delay = rand() % 3; //introduce random delay to simulate network updates
        sleep(delay);

        //check if any router's vector has changed
        bool changed = false;
        for (int i = 0; i < MAX_ROUTERS; ++i) {
            if (vectorChanged[i]) {
                changed = true;
                break;
            }
        }

        if (!changed) {
            break; // exit loop if no router's vector has changed
        }

    } while (true); // continue indefinitely until no router's vector changes

    printf("Router %d finishes recalculating its distance vector.\n", routerID);
    return NULL;
}

int main() {
    initializeNetwork(); //initialize network topology
    displayDistanceVectors(); //display initial distance vectors

    pthread_t threads[MAX_ROUTERS]; //threads for each router
    int routerIDs[MAX_ROUTERS];

    for (int i = 0; i < MAX_ROUTERS; ++i) {
        routerIDs[i] = i;
        pthread_mutex_init(&lock[i], NULL); //initialize mutex lock for each router
        pthread_create(&threads[i], NULL, simulateDVAlgorithm, (void *)&routerIDs[i]); //create thread for each router
    }
    
    for (int i = 0; i < MAX_ROUTERS; ++i) {
        pthread_join(threads[i], NULL); //wait for all threads to finish
        pthread_mutex_destroy(&lock[i]); //destroy mutex lock for each router
    }
    
    return 0;
}
