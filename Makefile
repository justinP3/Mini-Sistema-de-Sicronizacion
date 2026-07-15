# Nombre del ejecutable
TARGET = minisync

# Compilador y flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread -lrt -I./headers

# Lista de archivos objeto (deben coincidir con tus .c)
OBJS = monitor.o stats.o scaner.o worker.o logger.o copiador.o

# Regla principal
all: $(TARGET)

# Cómo construir el ejecutable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

# Cómo construir cada objeto
monitor.o: monitor.c
	$(CC) $(CFLAGS) -c monitor.c -o monitor.o

stats.o: stats.c
	$(CC) $(CFLAGS) -c stats.c -o stats.o

scaner.o: scaner.c
	$(CC) $(CFLAGS) -c scaner.c -o scaner.o

worker.o: worker.c
	$(CC) $(CFLAGS) -c worker.c -o worker.o

logger.o: logger.c
	$(CC) $(CFLAGS) -c logger.c -o logger.o

copiador.o: copiador.c
	$(CC) $(CFLAGS) -c copiador.c -o copiador.o

# Limpieza
clean:
	rm -f $(OBJS) $(TARGET) minisync.log