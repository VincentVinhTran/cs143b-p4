CC			= gcc
CFLAGS		= -Wall -std=c99
LDFLAGS 	= 
OBJFILES 	= fs.o if.o main.o shell.o
TARGET 		= fs

all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)

clean:
	/bin/rm $(OBJFILES) $(TARGET) *core*
