#include "./headers/stats.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>

int copiar (char* ruta_origen, char* ruta_destino){
    int fd_origen, fd_guardado;
    char buffer[4096];
    ssize_t cant_bytes;
    long bytes_totales = 0;
    struct stat stat_origen;
     //se obtiene los metadatos del archivo original
    if (stat(ruta_origen, &stat_origen) == -1) {
        write(2, "Error stat origen\n", 18);
        sumar_error();
        return -1;
    }
    //se abre el archivo del que copiar
    fd_origen = open(ruta_origen, O_RDONLY);
    if (fd_origen == -1) {
        write(2, "\nerror al abrir el archivo de origen en copiador", 49);
        sumar_error();
        return -1;
    }
    // se abre el archivo a donde se copia
    fd_guardado = open(ruta_destino, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_guardado == -1) {
        write(2, "error al abrir el archivo dde destino en copiador", 49);
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
    //copia los permisos del archivo ariginal
    if (chmod(ruta_destino, stat_origen.st_mode) == -1) {
        write(2, "Error al copia permisos\n", 12);
        sumar_error();
    }
    close(fd_guardado);
    close(fd_origen);
    sumar_copiado(bytes_totales);
    return 0;
}