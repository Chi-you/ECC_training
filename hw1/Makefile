all:
	g++ -o build1 hw1-1.cpp
	g++ -o build2 hw1-2.cpp
	./build1
	./build2
	python plot_cmp.py

hw1-1:
	g++ -o build hw1-1.cpp
	./build
	python plot.py Result1.txt

hw1-2:
	g++ -o build hw1-2.cpp
	./build
	python plot.py Result2.txt

plot: 
	./build
	python plot.py

clean:
	rm *.o *.exe build1 build2 *.txt
