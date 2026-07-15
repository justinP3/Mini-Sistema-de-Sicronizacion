#include "./headers/stats.h"
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

//estructura de los metadatos de los archivos a guardar
struct metadatos_file {
    char* ruta;
    ino_t inodo;
    off_t tamanio;
    mode_t permisos_tipo;
    time_t fecha_modificacion;
};
//asignacion dinamica de memoria
struct metadatos_file *memoria_metadatos = NULL;
int cant_usada;
int cant_actual;

//funcion para recorrer todo el directorio y leer su contenido
void scan_dir (char* ruta) {
    struct dirent* direccion_file;
    char ruta_guardada[1024];
    struct stat datos_temporal;
    // abrir el directorio a copiar
    DIR* carpeta = opendir(ruta);
    if (carpeta == NULL) {
        write(1, "error al abrir directorio a copiar scaner", 42);
        sumar_error();
        return;
    }
    //leer y obtener el directorio
    while ((direccion_file = readdir(carpeta)) != NULL) {
        //no leer el . y .. dentro de la carpeta para evitar bucles infinitos
        if (strcmp(direccion_file->d_name, ".") == 0 || strcmp(direccion_file->d_name, "..") == 0) {
            continue;
        }
        //obtener la ruta completa del archivo para guargar
        strcpy(ruta_guardada, ruta);
        strcat(ruta_guardada, "/");
        strcat(ruta_guardada, direccion_file->d_name);
        //obtener los metadatos
        if (lstat(ruta_guardada, &datos_temporal) == -1) {
            write(1, "error en lstat, obtener elementos scaner", 41);
            sumar_error();
            continue;
        }
        //aumentar meoria para guarda 10 structuras mas si falta para que no pida tan seguido
        if (cant_usada >= cant_actual) {
            cant_actual += 10;
            struct metadatos_file *memoria_temporal = realloc(memoria_metadatos, cant_actual * sizeof(struct metadatos_file));
            if (memoria_temporal == NULL) {
                write(1, "error al aumentar memoria para metadatos en scaner", 51);
                sumar_error();
                break;
            }
            memoria_metadatos = memoria_temporal;
        }
        //guardar los datos importantes
        memoria_metadatos[cant_usada].ruta = malloc(strlen(ruta_guardada)+1);
        strcpy(memoria_metadatos[cant_usada].ruta, ruta_guardada);
        memoria_metadatos[cant_usada].inodo = datos_temporal.st_ino;
        memoria_metadatos[cant_usada].tamanio = datos_temporal.st_size;
        memoria_metadatos[cant_usada].permisos_tipo = datos_temporal.st_mode;
        memoria_metadatos[cant_usada].fecha_modificacion = datos_temporal.st_mtime;
        cant_usada++;
        //recursividad si encuentra una carpeta
        if (S_ISDIR(datos_temporal.st_mode)) {
            scan_dir(ruta_guardada);
        }
    }
    closedir(carpeta);
}
//funcion para liberar la memoria usada
void liberar_escaner() {
    for (int i = 0; i < cant_usada; i++) {
        free(memoria_metadatos[i].ruta);
    }
    free(memoria_metadatos);
    memoria_metadatos = NULL;
    cant_usada = 0;
    cant_actual = 0;
}


