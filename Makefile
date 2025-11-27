all: ring_sum

ring_sum: ring_sum.cpp
	mpic++ -O2 -o ring_sum ring_sum.cpp

clean:
	rm -f ring_sum
