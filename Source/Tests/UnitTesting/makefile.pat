all:$(wildcard *.cpp)
	mkdir -p ./<!--Perl: config-->
	rc <!--Perl: config_defs --> /i <!--Perl: u3dsp-->RTL/Kernel/Include /fo ./<!--Perl: config-->/SamplePlayer.res <!--Perl: u3dsp-->Samples/SampleViewerWin32/SamplePlayer.rc
	cl $(filter-out , $(wildcard *.cpp)) @cl.txt
	link @link.txt