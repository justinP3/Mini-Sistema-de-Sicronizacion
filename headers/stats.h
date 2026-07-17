#ifndef STATS_H
#define STATS_H
#include <semaphore.h>
// structura stats
struct stats {
  long archivos_copiados;
  long bytes_copiados;
  long errores;
};
// funciones
void iniciar_stats();
void sumar_copiado(long bytes);
void sumar_error();
struct stats *obtener_stats();

#endif