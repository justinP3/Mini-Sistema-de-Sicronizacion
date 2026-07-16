#include "./headers/stats.h"
#include "./headers/logger.h"
#include "./headers/scaner.h"
#include "./headers/worker.h"
#include <libgen.h>
#include <mqueue.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define NUM_WORKERS 2 

// Función para imprimir números del stat
void escribir_numero(long numero) {
    char buffer[32];
    int i = 0;
    if (numero == 0) {
        write(1, "0", 1);
        return;
    }
    // extrae los dígitos
    while (numero > 0) {
        buffer[i] = (numero % 10) + '0';
        numero /= 10;
        i++;
    }
    // imprime los dígitos
    for (int j = i - 1; j >= 0; j--) {
        write(1, &buffer[j], 1);
    }
}

void monitor(int* tuberia, const char* ruta_origen, const char* ruta_destino) {
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
    chdir("/");
    // se crean la cantidad de Workers para que ejecuten el copiado cada uno
    for (int i = 0; i < NUM_WORKERS; i++) {
        pid_t pid_worker = fork();
        if (pid_worker == 0) {
            close(tuberia[1]); 
            ejecutar_worker(tuberia[0], ruta_destino);
            exit(0);
        }
    }
    close(tuberia[0]);
    //se realia el proceso para backup de la carpeta elejida
    char ruta_backup[2048];
    struct stat stat_backup;
    //array para saber que archivos ya se enviaron en el ciclo
     char archivos_enviados[100][1024];
    int cant_enviados = 0;
    //bucle infinito que sincronizara cada 5 segundos la carpeta
    while (1) {
        liberar_escaner();
        cant_enviados = 0;
        //se escane la carpeta
        scan_dir((char*)ruta_origen);
        //bucle for hasta que termine de revisar todos los files encontrados
        for (int i = 0; i < cant_usada; i++) {
            // se obtiene el nombre del archivo actual del arreglo del scaner
            char ruta_compr_temporal[1024];
            strncpy(ruta_compr_temporal, memoria_metadatos[i].ruta, sizeof(ruta_compr_temporal) - 1);
            ruta_compr_temporal[sizeof(ruta_compr_temporal) - 1] = '\0';
            char* nombre_file = basename(ruta_compr_temporal);
            // se construye la ruta en el directorio de backup
            strcpy(ruta_backup, ruta_destino);
            strcat(ruta_backup, "/");
            strcat(ruta_backup, nombre_file);
            // busca si el archivo ya esta en el back up para copiarlo directamente
            int necesita_copia = 0;
            if (stat(ruta_backup, &stat_backup) == -1) {
                necesita_copia = 1;
            } else {
                // en caso de existir se compara la fecha para actualizarlo o no
                if (memoria_metadatos[i].fecha_modificacion > stat_backup.st_mtime || memoria_metadatos[i].tamanio != stat_backup.st_size) {
                    necesita_copia = 1;
                }
            }
            // se realiza la copia en back up si necesita enviando el nombre del archivo al worker
            if (necesita_copia) {
                int ya_enviado = 0;
                for (int j = 0; j < cant_enviados; j++) {
                    if (strcmp(archivos_enviados[j], memoria_metadatos[i].ruta) == 0) {
                        ya_enviado = 1;
                        break;
                    }
                }
                //verificar que no se envio ya en el ciclo
                if (!ya_enviado) {
                    
                    write(tuberia[1], memoria_metadatos[i].ruta, strlen(memoria_metadatos[i].ruta) + 1);
                
                    // ← Registrar que lo enviamos
                    strcpy(archivos_enviados[cant_enviados], memoria_metadatos[i].ruta);
                    cant_enviados++;
                }
            }
        }
        // Los workers actualizan las estadísticas y el monitor las muestra en consola
        struct stats* est = obtener_stats();
        write(1, "\n ---Estadísticas Actuales:\n", 30);
        write(1, "  Archivos copiados: ", 21);
        escribir_numero(est->archivos_copiados);
        write(1, "\n", 1); 
        write(1, "  Bytes copiados: ", 18);
        escribir_numero(est->bytes_copiados);
        write(1, "\n", 1); 
        write(1, "  Errores: ", 11);
        escribir_numero(est->errores);
        write(1, "\n", 1);

        sleep(5); 
    }
}

int main(int argc, char** argv) {
    shm_unlink("/sync_stats");
    mq_unlink("/sync_log");
    unlink("minisync.log");

    // varificar y mostrar si faltan argunmentos
    if (argc != 3) {
        write(1,"Formato a usar: ./minisync <directorio_origen> <directorio_backup>\n", 68 );
        return 1;
    }
    //se incializa el log y stats
    iniciar_stats(); 
    iniciar_logger();
    //se hace fork para que un proceso entre en el if y se quede ejecutando la funcion
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
    monitor(tuberia, argv[1], argv[2]);
}