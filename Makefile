vm: vm.o
	cc -o vm.out vm.o
vm.o: vm.c
	cc -c -g vm.c

.PHONY: clean
clean:
	rm -f vm.out *.o