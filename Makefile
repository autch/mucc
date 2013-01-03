
TARGET = mucc

all: $(TARGET)

mucc: codegen.o hash.o mmlcv.o msrcv.o mucc.o reserved.o strsub.o
	$(CC) -o $@ $(LDFLAGS) $^ -lm

clean:
	rm -f *.o

