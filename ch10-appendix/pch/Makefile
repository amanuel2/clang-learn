# header.pch: header.h
# 	clang++ -x c++-header header.h -o header.pch 


HEADERS=$(wildcard ./*.h)
PCHS=$(HEADERS:.h=.pch)
CLANG=../../../llvm-project/install/bin/clang

%.pch: %.h
	$(CLANG) -cc1 -emit-pch -x c++-header $< -o $@

pch: $(PCHS)

main.o: main.cc pch
	$(CLANG) -cc1 -emit-obj -include-pch header.pch main.cc -o main.o

exe: main.o
	$(CLANG) main.o -o main -lstdc++

exenopch: main.cc
	$(CLANG) main.cc -o main -lstdc++


all: pch exe

clean:
	rm -rf *~ *.dSYM *.pch main
