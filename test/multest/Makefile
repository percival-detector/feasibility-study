CPP = g++
FLAG = -g -Wall
INC = -I/dls_sw/prod/tools/RHEL6-x86_64/boost/1-48-0/prefix/include
LIB = -L/dls_sw/prod/tools/RHEL6-x86_64/boost/1-48-0/prefix/lib
LD = -lboost_system

multest: multest.cpp
	$(CPP) $(FLAG) $(INC) -o multest multest.cpp $(LIB) $(LD)

