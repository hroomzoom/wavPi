
TARGET = wavpi
LIBS = -lsndfile -lasound -lpthread -lm -lncurses

SRCDIR = src
INCDIR = include
ARM_INCDIR = arm_include
BINDIR = /usr/local/bin
SOURCES = $(wildcard $(SRCDIR)/*.c)

CC = gcc
CFLAGS = -Wall -Wextra -I$(INCDIR) -I$(ARM_INCDIR)

.PHONY: all clean install uninstall

all: $(TARGET) install

$(TARGET): $(SOURCES)
	@mkdir -p build
	$(CC) $(CFLAGS) $^ $(LIBS) -o build/$@

install: $(TARGET)
	@echo "Installing $(TARGET) to $(BINDIR)..."
	install -D build/$(TARGET) $(BINDIR)/$(TARGET)

uninstall:
	rm -f $(BINDIR)/$(TARGET)

clean:
	rm -rf build