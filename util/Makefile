
CFLAGS = -O2 -Wall -Wextra -Wmissing-prototypes -Wstrict-prototypes -Werror=implicit-function-declaration -Werror=missing-prototypes

.PHONY: all clean

all : armid

armid : clean armid.c
	cc -o armid armid.c

clean :
	@rm -f armid
