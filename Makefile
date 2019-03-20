trivializer = trivializer

inc = .

all:
	gcc -std=c99 -I$(inc) -pthread $(trivializer).c -o $(trivializer).exe
	
clean:
	rm *.exe
