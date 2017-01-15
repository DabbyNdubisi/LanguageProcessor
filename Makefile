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

parser: Tokenizer.o POSTagger.o POSPerceptron.o POSAveragePerceptron.o DPPerceptron.o DPAveragePerceptron.o Parse.o SyntaxParser.o main.o
		$(CC) -std=c++11 $(LIB_FLAG) -o parser Tokenizer.o POSTagger.o POSPerceptron.o POSAveragePerceptron.o DPPerceptron.o DPAveragePerceptron.o Parse.o SyntaxParser.o main.o

POSTagger.o: POSTagger.h POSTagger.cpp Tokenizer.h errorCodes.h
	$(CC) -std=c++11 $(LIB_FLAG) -c POSTagger.cpp

main.o: main.cpp POSTagger.h errorCodes.h
	$(CC) -std=c++11 $(LIB_FLAG) -c main.cpp

Tokenizer.o: Tokenizer.h Tokenizer.cpp errorCodes.h
	$(CC) -std=c++11 $(LIB_FLAG) -c Tokenizer.cpp

POSPerceptron.o: POSPerceptron.h POSPerceptron.cpp
	$(CC) -std=c++11 $(LIB_FLAG) -c POSPerceptron.cpp

POSAveragePerceptron.o: POSAveragePerceptron.h POSAveragePerceptron.cpp
	$(CC) -std=c++11 $(LIB_FLAG) -c POSAveragePerceptron.cpp

DPPerceptron.o: DPPerceptron.h DPPerceptron.cpp
	$(CC) -std=c++11 $(LIB_FLAG) -c DPPerceptron.cpp

DPAveragePerceptron.o: DPAveragePerceptron.h DPAveragePerceptron.cpp
	$(CC) -std=c++11 $(LIB_FLAG) -c DPAveragePerceptron.cpp

SyntaxParser.o:	SyntaxParser.h SyntaxParser.cpp
	$(CC) -std=c++11 $(LIB_FLAG) -c SyntaxParser.cpp

Parse.o:	Parse.h Parse.cpp
	$(CC) -std=c++11 $(LIB_FLAG) -c Parse.cpp

clean:
	rm parser *.o
