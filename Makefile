# author: Pavol Babjak - xbabja03; Lukáš Václavek - xvacla32
# Projekt #13. SHO v logistice

ZIP_NAME = 13_xbabja03_xvacla32.zip

all:
	g++ -Wall -pedantic -std=c++11 main.cpp -o ims -lsimlib -lm

build:
	g++ -Wall -pedantic -std=c++11 main.cpp -o ims -lsimlib -lm
run:
	./ims

clean:
	rm -f ims *.o $(ZIP_NAME)

zip:
	zip -r $(ZIP_NAME) main.cpp Makefile Technicka_sprava.pdf README.md