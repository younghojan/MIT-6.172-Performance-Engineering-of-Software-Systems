CC := gcc
CFLAGS := -Wall -Werror -std=gnu99 -gdwarf-3 

ifeq ($(DEBUG),1)
	CFLAGS := -O0 -DDEBUG -g -ggdb $(CFLAGS)
else
	CFLAGS := -O2 -DNDEBUG $(CFLAGS)
endif

LDFLAGS := -lm
ifeq ($(PROFILE),1)
LDFLAGS := -g -lprofiler -Wl,-no_pie -fno-omit-frame-pointer -Wl,-export_dynamic $(LDFLAGS)
endif
LDFLAGS := $(LDFLAGS)

all: isort sum

isort.o: isort.c 
	$(CC) $(CFLAGS) -c isort.c 

qsort.o: qsort.c 
	$(CC) $(CFLAGS) -c qsort.c 

isort: isort.o qsort.o
	$(CC) -o isort isort.o qsort.o $(LDFLAGS)

isort_prof: isort.o qsort.o
	$(CC) -o isort isort.o qsort.o $(LDFLAGS)
	CPUPROFILE=isort.prof ./isort 10000 10
	PPROF_BINARY_PATH=/Users/SimonAertssen/Desktop/Zelfstudie/6.172\ Performance\ Engineering\ of\ Software\ Systems/MIT-6.172-Performance-Engineering-of-Software-Systems/Week\ 2\ Bentleys\ rules\ and\ bit\ hacks/recitation/
	pprof --text isort isort.prof

sum.o: sum.c 
	$(CC) $(CFLAGS) -c sum.c 

sum: sum.o
	$(CC) -o sum sum.o $(LDFLAGS)

clean:
	rm -f isort sum *.o *.gcda *.gcno *.gcov perf.data */perf.data cachegrind.out.*
