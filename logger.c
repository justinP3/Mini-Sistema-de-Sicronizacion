#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <time.h>
#include <unistd.h>

#define COLA_NOMBRE "/sync_log"
#define TAMANO_MAX 512
#define LOG_FILE "minisync.log"

void iniciar_logger() {
    struct mq_attr atributos_cola;
    atributos_cola.mq_flags = 0;
    atributos_cola.mq_maxmsg = 10;
    atributos_cola.mq_msgsize = TAMANO_MAX;
    atributos_cola.mq_curmsgs = 0;
    //se borra la cola antigua para evitar problemas
    mq_unlink(COLA_NOMBRE);
    // Creamos la cola POSIX (O_CREAT)
    mqd_t cola_mensajes = mq_open(COLA_NOMBRE, O_CREAT | O_RDWR, 0666, &atributos_cola);
    if (cola_mensajes == (mqd_t)-1) {
        write(2,"Error al crear la cola de mensajes del logger", 46);
        exit(1);
    }
    // se cierra la cola temporalmente
    mq_close(cola_mensajes); 
}
void ejecutar_logger() {
    // se abre la cola en modo lectura
    mqd_t cola_mensajes = mq_open(COLA_NOMBRE, O_RDONLY);
    if (cola_mensajes == (mqd_t)-1) {
        write(2, "Error al abrir cola\n", 20);
        exit(1);
    }
    //se abre el archivo de log
    int fd_log = open(LOG_FILE, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd_log == -1) {
        write(2, "Error al abrir archivo de log\n", 30);
        exit(1);
    }
    char buffer_mensaje[TAMANO_MAX];
    char linea_log[512];
    while (1) {
        // mq_receive bloquea al Logger hasta que un Worker envíe un mensaje
        ssize_t bytes_leidos = mq_receive(cola_mensajes, buffer_mensaje, TAMANO_MAX, NULL);
        if (bytes_leidos > 0) {
            // Nos aseguramos de que el texto termine correctamente
            buffer_mensaje[bytes_leidos] = '\0'; 
            // se obtiene la fecha y hora actual del sistema
            time_t tiempo_actual = time(NULL);
            struct tm *tiempo_info = localtime(&tiempo_actual);
            char texto_fecha[64];
            // formateo de fecha a 2023-11-20 16:45:00 e imprecion 
            strftime(texto_fecha, sizeof(texto_fecha), "%Y-%m-%d %H:%M:%S", tiempo_info);
            int tamano_linea = snprintf(linea_log, sizeof(linea_log), "[%s] copiado %s\n", texto_fecha, buffer_mensaje);
            write(fd_log, linea_log, tamano_linea);
        }
    }
}