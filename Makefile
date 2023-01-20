.DEFAULT_GOAL := intersect
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wvla -Wwrite-strings -Waggregate-return -Wfloat-equal
CFLAGS += -std=c18

intersect:

.PHONY: debug
debug: CFLAGS += -g
debug: intersect

.PHONY: profile
profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: intersect

# Will include proper test files once testing has been implemented
#.PHONY: check
#check: intersect

#test/test-all: LDFLAGS += -L.
#test/test-all: LDLIBS += -lcheck -lm -lrt -lpthread -lsubunit
#test/test-all: test/test-all.o test/test-intersect.o

.PHONY: clean
clean:
	$(RM) *.o intersect

