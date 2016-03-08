ifeq ($(CC),gcc)
	lib = libstdc++
	CC=g++
else
	lib = libc++
	CC=clang++
endif

parser: Tokenizer.o utility.o main.o
		$(CC) -std=c++11 -stdlib=$(lib) -o parser Tokenizer.o utility.o main.o

utility.o: utility.h utility.cpp Tokenizer.h
	$(CC) -std=c++11 -stdlib=$(lib) -c utility.cpp

main.o: main.cpp utility.h
	$(CC) -std=c++11 -stdlib=$(lib) -c main.cpp

Tokenizer.o: Tokenizer.h Tokenizer.cpp
	$(CC) -std=c++11 -stdlib=$(lib) -c Tokenizer.cpp

clean:
	rm parser *.o
