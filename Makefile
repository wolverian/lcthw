CFLAGS := --std=c11 -Wall -g

targets := ex1 ex3 ex4 ex5 ex6 ex7 ex8 ex9 ex10 ex12 ex14 ex15 ex16 ex16-stack ex17 stack

all: $(targets)

clean:
	rm -rf $(targets) *.dSYM
