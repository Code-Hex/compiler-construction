TEST = s-calc

CC = clang
CFLAGS = -g -O0 -Wall

TARGET = s-calc

s-calc: s-calc.c s-token.o
	$(CC) $(CFLAGS)  -o $@ $^

test:     $(TEST)
	./$(TEST) < s-input.txt

clean:
	rm -rf *.o  $(TARGET) a.out *.dSYM *.s

s-calc.o: s-compile.h
s-token.o: s-compile.h
