.DEFAULT_GOAL := intersect
CFLAGS += -Wall -Wextra -Wpedantic
CFLAGS += -Wvla -Wwrite-strings -Waggregate-return -Wfloat-equal
CFLAGS += -std=c18

intersect: intersect.o hashset.o

.PHONY: debug
debug: CFLAGS += -g
debug: intersect

.PHONY: profile
profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: intersect

# If this doesn't run, check the executable bit on test.bash
.PHONY: check
check: intersect
check:
	./test/test.bash


.PHONY: clean
clean:
	$(RM) *.o intersect

