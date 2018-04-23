test:
	g++ bp.cpp -o bp --std=c++11 -Ofast -pthread
	rm bp
debug:
	g++ bp.cpp -o bp --std=c++11 -Ofast -pthread
	./bp
	rm bp