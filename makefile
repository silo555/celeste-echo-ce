NAME = CECHO
ICON = icon.png
DESCRIPTION = "CEleste Echo"
COMPRESSED = YES
ARCHIVED = YES
CFLAGS = -Wall -Wextra -Oz -DNDEBUG
CXXFLAGS = -Wall -Wextra -Oz -DNDEBUG
include $(shell cedev-config --makefile)
