#ifndef SCANER_H
#define SCANER_H

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

// estructura de metadatos
struct metadatos_file {
  char *ruta;
  ino_t inodo;
  off_t tamanio;
  mode_t permisos_tipo;
  time_t fecha_modificacion;
};
// se declara las variables globales
extern struct metadatos_file *memoria_metadatos;
extern int cant_usada;
extern int cant_actual;
// funciones
void scan_dir(char *ruta);
void liberar_escaner();

#endif