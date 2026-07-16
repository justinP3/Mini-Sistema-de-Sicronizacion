#include "./headers/copiador.h"
#include "./headers/stats.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <mqueue.h>
#include <libgen.h>

//funcion para crear los directorios necesarios para el archivo a copiar
void crear_directorios(const char* ruta_completa) {
    char ruta_temporal[2048];
    strcpy(ruta_temporal, ruta_completa);
    for (int i = 1; ruta_temporal[i] != '\0'; i++) {
        // obtiene direcotrio por directorio creandolo en caso de no existir en backup
        if (ruta_temporal[i] == '/') {
            ruta_temporal[i] = '\0';
            mkdir(ruta_temporal, 0755);
            ruta_temporal[i] = '/';
        }
    }
}

void ejecutar_worker(int pipe_lectura, const char *ruta_destino, const char *ruta_origen) {
    char ruta_a_copiar[1024];
    char ruta_guardado[2048];
    char ruta_relativa[1024];
    //se abre una cola de mensajes para logger
    mqd_t msg_log = mq_open("/sync_log", O_WRONLY);
    if (msg_log == (mqd_t)-1) {
        write(1, "error creando la cola logger el worker", 39);
        sumar_error();
    }
    //bucle que se ejecuta cuando el monitor lo manda a hacer copias
    while (read(pipe_lectura, ruta_a_copiar, sizeof(ruta_a_copiar)) > 0) {
        // si no necesita de carpetas lo copia directo
        if (strncmp(ruta_a_copiar, ruta_origen, strlen(ruta_origen)) == 0) {
            strcpy(ruta_relativa, ruta_a_copiar + strlen(ruta_origen) + 1);
        } else {
            char copia_ruta[1024];
            strcpy(copia_ruta, ruta_a_copiar);
            char* nombre_archivo = basename(copia_ruta);
            strcpy(ruta_relativa, nombre_archivo);
        }
        strcpy(ruta_guardado, ruta_destino);
        strcat(ruta_guardado, "/");
        strcat(ruta_guardado, ruta_relativa);
        // crea los direcotrios
        crear_directorios(ruta_guardado);
        //se ejecuta el copiado del archivo
        if (copiar(ruta_a_copiar, ruta_guardado) == 0) {
            //envia el nombre del archivo al logger
            mq_send(msg_log, ruta_relativa, strlen(ruta_relativa) + 1, 0);
        }
    }
    //cerrar todos los archivos abiertos para finalizar
    if (msg_log != (mqd_t)-1) {
        mq_close(msg_log);
    }
    close(pipe_lectura);
    exit(0);
}