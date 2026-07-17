#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "./headers/stats.h"

static struct stats *stats = NULL;
static sem_t *semaforo = NULL;

void iniciar_stats(){
    int fd_stat = shm_open("/sync_stats", O_CREAT | O_RDWR, 0666);
    ftruncate(fd_stat, sizeof(struct stats));
    stats = mmap(NULL, sizeof(struct stats), PROT_READ | PROT_WRITE, MAP_SHARED, fd_stat, 0);
    semaforo = sem_open("/minisync_sem", O_CREAT, 0666, 1);
}
void sumar_copiado(long bytes){
    sem_wait(semaforo);
    stats->archivos_copiados++;
    stats->bytes_copiados += bytes;
    sem_post(semaforo); 
}
void sumar_error(){
    sem_wait(semaforo);
    stats->errores++;
    sem_post(semaforo);
}
struct stats* obtener_stats(){
    return stats;
}