_This project has been created as part of the 42 curriculum by <leodum> _

# Codexion

## Description

Codexion is a multithreaded simulation inspired by the classic **Dining Philosophers Problem**, reimagined in a software engineering context. Instead of philosophers and forks, the simulation features **coders** competing for **dongles** — hardware USB license keys required to compile code.

Each coder follows a fixed lifecycle:

1. Acquire two dongles (left and right)
2. Compile (while holding both dongles)
3. Release dongles (which enter a cooldown period)
4. Debug
5. Refactor
6. Repeat until the required number of compiles is reached — or burn out

The simulation ends either when all coders have compiled the required number of times, or when a coder **burns out** — meaning they did not start compiling within `time_to_burnout` milliseconds since their last compile or since the simulation began.

A dedicated **monitor thread** runs in parallel, checking for burnout conditions and signaling all threads to stop cleanly when the simulation ends.

The project explores core concurrent programming concepts: thread creation and synchronization, mutex locks, condition variables, deadlock prevention, starvation avoidance, and precise timing.


## Instructions

### Requirements

- `cc` compiler
- POSIX threads (`-pthread`)
- Linux or macOS

### Compilation

```bash
make
```

### Execution

```bash
./codexion nb_coders time_to_burnout time_to_compile time_to_debug time_to_refactor nb_of_compiles dongle_cooldown scheduler
```

All time values are in **milliseconds**. The scheduler must be exactly `fifo` or `edf`.

### Example

```bash
./codexion 4 800 200 100 100 5 50 fifo
```

### Makefile rules

| Rule                      | Description                    |
| ------------------------- | ------------------------------ |
| `make`                    | Compile the project            |
| `make clean`              | Remove object files            |
| `make fclean`             | Remove object files and binary |
| `make re`                 | Full recompile                 |
| `make test`               | Run all predefined test cases  |

### Arguments

| Argument           | Description                                               |
| ------------------ | --------------------------------------------------------- |
| `nb_coders`        | Number of coders and dongles (max ~200)                   |
| `time_to_burnout`  | Max ms between compile starts before burnout              |
| `time_to_compile`  | Duration of compile phase in ms                           |
| `time_to_debug`    | Duration of debug phase in ms                             |
| `time_to_refactor` | Duration of refactor phase in ms                          |
| `nb_of_compiles`   | Number of compiles required per coder                     |
| `dongle_cooldown`  | Ms a dongle is unavailable after release                  |
| `scheduler`        | `fifo` (arrival order) or `edf` (earliest deadline first) |

### Log format

Every state change is printed as:

```
timestamp_in_ms coder_number state
```

Example:

```
0 1 has taken a dongle
2 1 has taken a dongle
2 1 is compiling
202 1 is debugging
402 1 is refactoring
405 2 has taken a dongle
```


## Blocking Cases Handled

### Deadlock prevention — asymetric solution

The classic dining philosophers deadlock occurs when every coder holds their left dongle and waits for their right. Codexion prevents this using the **asymetric solution solution**: even coders take one direction first, odd coders the other.  

```
if (coder->nb % 2 == 0) 
    take left first, then right
else
    take right first, then left
```

This breaks the **circular wait** condition from Coffman's four necessary conditions for deadlock (mutual exclusion, hold and wait, no preemption, circular wait).

### Starvation prevention — Scheduler

Without a scheduler, `pthread_cond_broadcast` wakes all waiting coders and the OS decides who proceeds — which can cause starvation where some coders never get both dongles.

Codexion uses a **per-dongle min-heap** to enforce fair ordering:

- **FIFO**: coders are ordered by `arrival_ms`, which increments each time they take a dongle. Coders who have waited longest have the lowest rank and are served first.
- **EDF**: coders are ordered by deadline (`last_compile_start + time_to_burnout`). The coder closest to burning out is served first.

A coder only proceeds when they are at the top of the heap for that dongle, preventing indefinite starvation.

### Cooldown handling

After a dongle is released, it enters a cooldown period during which it cannot be acquired. The dongle's `available_to_use` field stores the absolute timestamp when it becomes available again. The monitor thread broadcasts on each dongle's condition variable once cooldown has passed, waking any waiting coders to retry.

### Precise burnout detection

A dedicated monitor thread checks all coders every millisecond. Burnout is detected when:

```
get_time_ms() - coder.last_time_compiled > time_to_burnout
```

The burnout message is guaranteed to appear within ~2ms of the actual event (1ms monitor sleep + scheduling overhead), well within the 10ms requirement. Upon detection, the monitor sets the shared `ongoing` flag and broadcasts on all dongle condition variables to wake any stuck threads.

### Log serialization

All state messages are protected by a shared `print_message` mutex to prevent interleaving of output from concurrent threads. Every print goes through `print_status`, which locks before writing and unlocks immediately after, ensuring atomic log entries.


## Thread Synchronization Mechanisms

### `pthread_mutex_t` — Mutual exclusion

Three categories of mutexes are used:

**Per-dongle mutex (`DongleLock`)**: protects `dongle->status`, `dongle->available_to_use`, and the dongle's heap. Any thread reading or writing dongle state must hold this lock.

**Per-coder mutex (`CoderLock`)**: protects `coder->last_time_compiled` and `coder->nb_of_compiles`. The monitor reads these fields while coder threads write them — the lock prevents torn reads.

**Shared print mutex (`print_message`)**: serializes all output. Also protects the `sim->ongoing` flag, ensuring the stop signal is published and read atomically.

### `pthread_cond_t` — Condition variables

Each dongle has a `condDongle` condition variable paired with its `DongleLock`. Coders that cannot immediately acquire a dongle call `pthread_cond_wait`, atomically releasing the lock and sleeping until signaled.

Three events trigger a broadcast on a dongle's condition variable:

- A dongle is released (`release_dongle`)
- A dongle's cooldown expires (monitor thread)
- The simulation ends (monitor's `stop_simulation`)

The lock-then-check pattern prevents TOCTOU races:

```c
pthread_mutex_lock(&dongle->DongleLock);
while (dongle->status != 0 || !check_simulation_ongoing(sim))
    pthread_cond_wait(&dongle->condDongle, &dongle->DongleLock);
// safely take dongle here
pthread_mutex_unlock(&dongle->DongleLock);
```

### Custom scheduling — per-dongle min-heap

Rather than letting the OS decide which waiting coder proceeds after a broadcast, Codexion implements a custom scheduler using a min-heap per dongle. After waking from `pthread_cond_wait`, a coder checks whether it is at the top of the heap before proceeding. If not, it waits again. This ensures FIFO or EDF ordering is respected regardless of OS thread scheduling.

### Monitor thread

A single monitor thread runs alongside all coder threads. It polls every 1ms and:

- Checks each coder's burnout condition
- Broadcasts on dongles whose cooldown has expired
- Checks whether all coders have completed their required compiles

When stopping the simulation, the monitor sets `ongoing = 1` under the print mutex, then broadcasts on every dongle condition variable to wake all blocked threads so they can exit cleanly.



## Resources

### General
- Yassir El Bakkari - Codexion - https://dev.to/yel-bakk/codexion-4fk8
- Dining Philosophers Problem - https://en.wikipedia.org/wiki/Dining_philosophers_problem

### Thread Management
- GeeksForGeek - Thread Management Function in C : https://www.geeksforgeeks.org/c/thread-functions-in-c-c/
- GeeksFor Geek - Concurency problems in DBMS Transactions : https://www.geeksforgeeks.org/dbms/concurrency-problems-in-dbms-transactions/
- LLNL HPC Tutorials - POSIX Threads Programming : https://hpc-tutorials.llnl.gov/posix/
- CodeVault - Short introduction to threads (pthreads): https://www.youtube.com/watch?v=d9s_d28yJq0&list=PLaRCVIXVlHUQQUQABwr2-oK9kjMcO_2vX&index=5&t=8s
- Portfolio Courses - Introduction to threads (pthread) | C Programming tutorial: https://www.youtube.com/watch?v=ldJ8WGZVXZk&list=PLaRCVIXVlHUQQUQABwr2-oK9kjMcO_2vX&index=5&t=4s
- Understanding thread in C, https://medium.com/@akshatarhabib/understanding-threads-in-c-c9feb5e9372a
- Threads, Mutexes and Concurrent Programming in C, https://www.codequoi.com/en/threads-mutexes-and-concurrent-programming-in-c/
- Race Conditions: the silent thread in Concurrent systems,  https://medium.com/@arunseetharaman/race-conditions-the-silent-threat-in-concurrent-systems-11c440bd115d
- Deadlock management  https://en.wikipedia.org/wiki/Deadlock_(computer_science)

### Data Structures and Binary Heap
- Queue in C, https://www.geeksforgeeks.org/c/queue-in-c/
- Implementing a queue in C, https://www.youtube.com/watch?v=Ra6p-Bmajlw
- Understanding Queue Data Structure in C: The First In, First Out Principle, Noran Saber Abdelfattah - https://medium.com/@noransaber685/understanding-queue-data-structures-in-c-the-first-in-first-out-principle-fbd1f89d40dc 
- Implement a binary heap - An efficient implementation of the Priority Queue AFT (Abstract Data Type) - https://www.youtube.com/watch?v=g9YK6sftDi0
- Binary Heap - https://www.geeksforgeeks.org/dsa/binary-heap/
### AI usage

Claude (Anthropic) was used throughout this project as a **mentor and rubber duck**, specifically:

- **Conceptual guidance**: explaining threading primitives (mutexes, condition variables, TOCTOU races, lock ordering deadlocks), the resource hierarchy solution, and EDF/FIFO scheduling theory
- **Debugging assistance**: identifying bugs through targeted questions rather than direct code fixes — the AI asked diagnostic questions to guide independent discovery of issues
- **Design decisions**: discussing tradeoffs between data structure choices (per-dongle heap vs global queue, stack vs heap allocation, embedded vs pointer mutex)
- **Code review**: identifying issues such as missing unlocks, wrong operator precedence, uninitialized pointers, and off-by-one errors in loop conditions
- **No code was written by AI**: all implementation was done independently; AI interactions were limited to questions, pseudocode, and conceptual explanations per the project's learning objectives
