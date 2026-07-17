#include "./headers/stats.h"
#include "./headers/logger.h"
#include "./headers/scaner.h"
#include "./headers/worker.h"
#include <libgen.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define NUM_WORKERS 10 
#define RUTA_BACKUP "./backup"

void monitor(int* tuberia, const char* ruta_origen) {
    mkdir(RUTA_BACKUP, 0755);
    // se hace daemon
    pid_t pid_daemon = fork();
    if (pid_daemon < 0) {
        write(2,"Error al crear el demonio", 26);
        exit(1);
    }
    if (pid_daemon > 0) {
        exit(0);
    }
    setsid();
    // se crean la cantidad de Workers para que ejecuten el copiado cada uno
    for (int i = 0; i < NUM_WORKERS; i++) {
        pid_t pid_worker = fork();
        if (pid_worker == 0) {
            close(tuberia[1]); 
            ejecutar_worker(tuberia[0], RUTA_BACKUP, ruta_origen);
            exit(0);
        }
    }
    close(tuberia[0]);
    //se realia el proceso para backup de la carpeta elejida
    char ruta_backup[2048];
    struct stat stat_backup;

    //bucle infinito que sincronizara cada 5 segundos la carpeta
    while (1) {
        //se escanea la carpeta
        scan_dir((char*)ruta_origen);
        //bucle for hasta que termine de revisar todos los files encontrados
        for (int i = 0; i < cant_usada; i++) {
            // no copiar direcotrios
            if (!S_ISREG(memoria_metadatos[i].permisos_tipo))  continue;
            // se obtiene el nombre del archivo actual del arreglo del scaner y tambien salta archivos que no sean del directorio
            char ruta_relativa[1024];
            if (strncmp(memoria_metadatos[i].ruta, ruta_origen, strlen(ruta_origen)) == 0) {
                strcpy(ruta_relativa, memoria_metadatos[i].ruta + strlen(ruta_origen) + 1);
            } else continue;
            // se construye la ruta en el directorio de backup
            strcpy(ruta_backup, RUTA_BACKUP);
            strcat(ruta_backup, "/");
            strcat(ruta_backup, ruta_relativa);
            // busca si el archivo ya esta en el back up para copiarlo directamente
            int necesita_copia = 0;
            if (lstat(ruta_backup, &stat_backup) == -1) {
                necesita_copia = 1;
            } else {
                // en caso de existir se compara la fecha para actualizarlo o no
                if (memoria_metadatos[i].fecha_modificacion > stat_backup.st_mtime || memoria_metadatos[i].tamanio != stat_backup.st_size) {
                    necesita_copia = 1;
                }
            }
            // se realiza la copia en back up si necesita enviando el nombre del archivo al worker
            if (necesita_copia) {
                    write(tuberia[1], memoria_metadatos[i].ruta, strlen(memoria_metadatos[i].ruta) + 1);            
                }
            }
        // Los workers actualizan las estadísticas y el monitor las muestra en consola
        struct stats* est = obtener_stats();
        char buf[256];
        int len = snprintf(buf, sizeof(buf),
            "\n---Estadísticas Actuales:\n  Archivos copiados: %ld\n  Bytes copiados: %ld\n  Errores: %ld\n", 
            est->archivos_copiados, est->bytes_copiados, est->errores);
        write(1, buf, len);
        liberar_escaner();
        sleep(5); 
    }
}

int main(int argc, char** argv) {
    shm_unlink("/sync_stats");
    unlink("minisync.log");
    // varificar y mostrar si faltan argunmentos
    if (argc != 2) {
        write(1,"Formato correcto: ./minisync <directorio_origen>\n", 48 );
        return 1;
    }
    //se incializa el log y stats
    iniciar_stats(); 
    iniciar_logger();
    //se hace fork para quese quede ejecutando la funcion
    pid_t pid_logger = fork();
    if (pid_logger == 0) {
        ejecutar_logger();
        exit(0);
    }
    // se crean tuberías para comunicacion
    int tuberia[2];
    if (pipe(tuberia) == -1) {
        write(2,"Error creando tubería", 21);
        return 1;
    }
    // se ejecuta el monitor
    monitor(tuberia, argv[1]);
}
