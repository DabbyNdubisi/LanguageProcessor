ifeq "$(OS)" ""
	OS = $(shell uname -s)
endif

ifeq "$(OS)" "Darwin"
	LIB_FLAG = -stdlib=libc++
	CC=clang++
else
	LIB_FLAG = 
	CC=g++
endif

parser: Tokenizer.o POSTagger.o main.o
		$(CC) -std=c++11 $(LIB_FLAG) -o parser Tokenizer.o POSTagger.o main.o

POSTagger.o: POSTagger.h POSTagger.cpp Tokenizer.h errorCodes.h
	$(CC) -std=c++11 $(LIB_FLAG) -c POSTagger.cpp

main.o: main.cpp POSTagger.h errorCodes.h
	$(CC) -std=c++11 $(LIB_FLAG) -c main.cpp

Tokenizer.o: Tokenizer.h Tokenizer.cpp errorCodes.h
	$(CC) -std=c++11 $(LIB_FLAG) -c Tokenizer.cpp

clean:
	rm parser *.o output.txt
