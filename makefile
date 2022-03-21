ALL : SkipListTest.exe
myArgs = -lpthread  -g
SkipListTest.exe : ./main.cpp
	g++ $^ -o $@ $(myArgs)

clean:
	-rm -rf SkipListTest.exe 
.PHONY: clean ALL