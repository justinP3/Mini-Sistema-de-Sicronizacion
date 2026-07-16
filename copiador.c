#include "./headers/stats.h"
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>
#define LOG_DEBUG "minisync.log"

int copiar (char* ruta_origen, char* ruta_destino){
    int fd_debug = open(LOG_DEBUG , O_WRONLY | O_CREAT | O_APPEND, 0644);
    int fd_origen, fd_guardado;
    char buffer[4096];
    ssize_t cant_bytes;
    long bytes_totales = 0;
    //se abre el archivo del que copiar
    write(fd_debug, "\n ruta que recibe copiador:", 27);
    write (fd_debug, ruta_origen, strlen(ruta_origen));
    fd_origen = open(ruta_origen, O_RDONLY);
    if (fd_origen == -1) {
        write(2, "\nerror al abrir el archivo del cual copiar en copiador", 54);
        sumar_error();
        return -1;
    }
    // se abre el archivo a donde se copia
    fd_guardado = open(ruta_destino, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_guardado == -1) {
         write(2, "error al abrir el archivo donde copiar en copiador", 51);
        sumar_error();
        return -1;
    }
    //realiza el copiado del archivo por bloques de 4096
    while ((cant_bytes = read(fd_origen, buffer, 4096)) > 0) {
        if (write(fd_guardado, buffer, cant_bytes) != cant_bytes) {
            sumar_error();
            close(fd_origen);
            close(fd_guardado);
            return -1;
        }
        bytes_totales += cant_bytes;
    }
    close(fd_guardado);
    close(fd_origen);
    sumar_copiado(bytes_totales);
    return 0;
}