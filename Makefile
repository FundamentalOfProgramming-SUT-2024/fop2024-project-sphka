rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SOURCES := $(call rwildcard,src,*.c)
HEADERS := $(call rwildcard,src,*.h)

rogue: $(SOURCES) $(HEADERS)
	cc $(SOURCES) -o rogue -lncurses -g -Wall

run: rogue
	./rogue

clean:
	rm rogue
