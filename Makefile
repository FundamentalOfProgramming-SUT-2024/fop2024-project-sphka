rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SOURCES := $(call rwildcard,src,*.c)
HEADERS := $(call rwildcard,src,*.h)

rogue: $(SOURCES) $(HEADERS)
	clang $(SOURCES) -o rogue -lncursesw -g -Wall -I src

check: $(SOURCES) $(HEADERS)
	clang-tidy $(SOURCES) -checks=-*,clang-analyzer-*,-clang-analyzer-cplusplus*

run: rogue
	./rogue

clean:
	rm rogue
