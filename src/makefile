lily : lily.o parse.o tex.o symbol.o vm.o gen.o
	gcc lily.o parse.o tex.o symbol.o vm.o gen.o -o lily -g

lily.o : lily.c
	gcc -c lily.c -o lily.o -g
parse.o : parse.c
	gcc -c parse.c -o parse.o -g
tex.o : tex.c
	gcc -c tex.c -o tex.o -g
symbol.o : symbol.c
	gcc -c symbol.c -o symbol.o -g
vm.o : vm.c
	gcc -c vm.c -o vm.o -g
gen.o : gen.c
	gcc -c gen.c -o gen.o -g
clean:
	rm lily.o parse.o tex.o symbol.o vm.o gen.o  
