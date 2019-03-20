# Trivializer #
Run lines of system commands in parallel via master-and-worker POSIX threads

## Usage ##
`trivializer.exe [todo_file] [# of processes]`

### Example ###
`trivializer.exe todo.txt 4`

Suppose `todo.txt` has the following contents:
```
./validate_serial.sh americas/caribbean hti
./validate_serial.sh americas/caribbean pri
./validate_serial.sh americas/northern_america can
./validate_serial.sh americas/northern_america usa
./validate_serial.sh americas/south_america chl
./validate_serial.sh asia/western_asia tur
./validate_serial.sh europe/eastern_europe hun
./validate_serial.sh oceania/melanesia fji
```

Trivializer will group each line of `todo.txt` into sublists of commands to be distributed and performed in parallel by (4 processes - 1 master thread) worker threads:
```
TODO_LIST[0]=./validate_serial.sh americas/caribbean hti
TODO_LIST[1]=./validate_serial.sh americas/caribbean pri
TODO_LIST[2]=./validate_serial.sh americas/northern_america can
TODO_LIST[3]=./validate_serial.sh americas/northern_america usa
TODO_LIST[4]=./validate_serial.sh americas/south_america chl
TODO_LIST[5]=./validate_serial.sh asia/western_asia tur
TODO_LIST[6]=./validate_serial.sh europe/eastern_europe hun
TODO_LIST[7]=./validate_serial.sh oceania/melanesia fji

LINE_COUNT=8
WORKER_COUNT=3
EXTRA=2
QUOTA=2
Worker 0: [0 - 2]
Worker 1: [3 - 5]
Worker 2: [6 - 7]
```

So TODO_LIST[0], TODO_LIST[3], and TODO_LIST[6] should be scheduled to be the first commands to execute in parallel.
