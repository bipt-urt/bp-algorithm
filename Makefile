test:
	g++ bp.cpp -o bp --std=c++14
	rm bp
debug:
	g++ bp.cpp -o bp --std=c++14
	./bp
	rm bp