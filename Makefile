test:
	g++ bp.cpp -o bp --std=c++14 -Ofast
	rm bp
debug:
	g++ bp.cpp -o bp --std=c++14 -Ofast
	./bp
	rm bp