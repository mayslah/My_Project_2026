# My_Project_2026
**Team:** Bingo Logic (yes we have a team name for morale purposes)

* **Partner :** Maysa Lahaleh
  - ID Num: 214003873
  - GitHub: [mayslah](https://github.com/mayslah)
  - Emails: mays.lahaleh@gmail.com / lahalehma@post.jce.ac.il

## Repository Link
[https://github.com/mayslah/My_Project_2026]

[Watch the Project Demo Video Here](https://drive.google.com/file/d/1_rQYJ4V9WTZmXlUg1uaYJPX91jU8dFfU/view?usp=drive_link)

## Project Environment
* **OS:** Linux (Ubuntu/Debian)
* **Compiler:** gcc
* **Graphics Library:** Raylib (for Milestones 2+)

## Technical Instructions

### Compilation
We provide a `Makefile` with the following targets:
* `make milestone1`: Builds the Dijkstra executable (`dijkstra`).
* `make milestone2`: Builds the GUI visualization executable (`sim`).
* `make milestone3`: Builds the animation simulation executable (`sim`).
* `make clean`: Removes all compiled files.

### Execution
* **Milestone 1:** `./dijkstra <file_name>`
* **Milestones 2-3:** `./sim <file_name>`

## Implementation Overview
* **Milestone 1:** Core Dijkstra implementation using an Adjacency Matrix. It reads a graph and query from a text file, outputs the path with arrows, and handles edge cases such as unreachable nodes, source-equals-destination, and invalid negative weights.
* **Milestone 2:** Visualized the graph nodes, directed edges, and weights using **Raylib**.
* **Milestone 3:** Added a moving entity that follows the shortest path:
    * **Timing Logic:** 1-second pause at each intermediate node.
    * **Movement:** Segments edges into W steps, moving every 300ms.
    * **Controls:** Includes a Play/Stop button to control the animation.

 * **Milestone 7**: Scheduling Algorithms
In this milestone, the random order of entering a node was replaced with a scheduler.

The program supports two scheduling algorithms:

### FCFS
First Come First Served.  
The first traveler that requests to enter a node is the first one allowed to enter.

### SJF
Shortest Job First.  
When multiple travelers wait for the same node, the scheduler chooses the traveler with the shortest remaining path cost.

### Compilation

```bash
make milestone7
