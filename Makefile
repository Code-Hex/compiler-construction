TEST = s-imac64

CC = clang
CFLAGS = -g -O0  -Wall

TARGET =  s-imac64

s-imac64: s-token.o s-tree-compile.o s-code-intel64-mac.o  
	$(CC) $(CFLAGS)  -o $@ $^

clean:
	rm -rf *.o  $(TARGET) a.out *.dSYM *.s

test: $(TEST)
	./$(TEST) < s-input.txt > s-output.s
	$(CC) s-output.s
	./a.out

s-token.o: s-compile.h
s-tree-compile.o: s-compile.h
s-code-intel64-mac.o: s-compile.h