

Target = example
UT_DIR = ./ut
UTTest	= ut_test
CPP_LIB_DIR = ./lib 
LDLIBS = -lpthread
SRC = MsgDisTest.c ThreadPool.c MsgDispatch.c
UT_SRC = $(UT_DIR)/Test.c
OBG	=	$(SRC:.c=.o)
UT_OBG	=	ThreadPool.o

all: $(Target) $(UTTest)

$(Target): $(OBG)
	gcc -g -o $(Target) $(OBG) $(LDLIBS)

$(UTTest): $(UT_SRC) $(UT_OBG)
	g++ -o $(UTTest) $(UT_SRC)  $(UT_OBG) -lpthread -I CPP_LIB_DIR -l CppUTest
	
.PHONY : clean
clean:
	-rm -rf *.o
