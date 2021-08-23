dst=<!--Perl: u3dsp-->Tests/UnitTesting/<!--Perl: groupTestsName-->
fname=<!--Perl: TestName-->
obj=<!--Perl: OBJ-->

all:$(wildcard *.cpp)
	mkdir -p ./<!--Perl: config-->
	<!--Perl: compiler--> -c $(filter-out , $(wildcard *.cpp)) $(shell cat gcc.txt)
	cp -f *.o ./<!--Perl: config-->
	rm -f *.o
	g++ -Wl,$(obj) $(shell cat linkMac.txt) -ldl -lstdc++ -framework Carbon -o $(dst)/<!--Perl: config-->/$(fname).out

