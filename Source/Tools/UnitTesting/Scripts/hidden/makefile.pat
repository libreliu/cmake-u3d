all:$(wildcard *.cpp)
all:$(wildcard <!--Perl: u3dsp-->Tools/UnitTesting/Source/*.cpp)
	mkdir -p ./<!--Perl: config-->
	rc /d NDEBUG /i <!--Perl: u3dsp-->RTL/Kernel/Include /fo ./<!--Perl: config-->/SamplePlayer.res <!--Perl: u3dsp-->Samples/SceneHelpers/SamplePlayer.rc
	cl $(filter-out , $(wildcard *.cpp)) $(filter-out , $(wildcard <!--Perl: u3dsp-->Tools/UnitTesting/Source/*.cpp)) $(filter-out , $(wildcard <!--Perl: u3dsp-->RTL/IFXCorePluginStatic/*.cpp)) @cl.txt
	link @link.txt