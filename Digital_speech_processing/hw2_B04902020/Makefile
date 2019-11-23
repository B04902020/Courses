# The following two variable will be commandline determined by TA
# For testing, you could uncomment them.
SRIPATH  ?= /home/student/04/b04902020/srilm-1.5.10
MACHINE_TYPE ?= i686-m64
LM ?= bigram.lm

CXX = g++
CXXFLAGS = -O3 -I$(SRIPATH)/include

vpath lib%.a $(SRIPATH)/lib/$(MACHINE_TYPE)

TARGET = mydisambig
SRC = mydisambig.cpp
OBJ = $(SRC:.cpp=.o)
TO = ZhuYin-Big5.map
FROM = Big5-ZhuYin.map
.PHONY: all clean map run

all: $(TARGET)

$(TARGET): $(OBJ) -loolm -ldstruct -lmisc
	$(CXX) $(LDFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

myexec=./mydisambig -lm bigram.lm -map ZhuYin-Big5.map
run:
	$(myexec) < ./testdata/1.txt > ./result2/1.txt
	$(myexec) < ./testdata/2.txt > ./result2/2.txt
	$(myexec) < ./testdata/3.txt > ./result2/3.txt
	$(myexec) < ./testdata/4.txt > ./result2/4.txt
	$(myexec) < ./testdata/5.txt > ./result2/5.txt
	$(myexec) < ./testdata/6.txt > ./result2/6.txt
	$(myexec) < ./testdata/7.txt > ./result2/7.txt
	$(myexec) < ./testdata/8.txt > ./result2/8.txt
	$(myexec) < ./testdata/9.txt > ./result2/9.txt
	$(myexec) < ./testdata/10.txt > ./result2/10.txt

map:
	python mapping.py Big5-ZhuYin.map ZhuYin-Big5.map

clean:
	$(RM) $(OBJ) $(TARGET)
	$(RM) -f *~

