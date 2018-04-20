test:
	g++ bp.cpp -o bp --std=c++11 -Ofast
	rm bp
debug:
	g++ bp.cpp -o bp --std=c++11 -Ofast
	./bp
	rm bp