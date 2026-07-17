# Proyecto Mini-Sistema-de-Sicronizacion
Este proyecto simula un sistema de soncronizacion de archivos que se ejecuta cada 5 segundos
## structura del proyecto
El proyecto tiene la sigueinte estructura
SICRONIZADOR  
├── headers  
│   ├── copiador.h  
│   ├── logger.h  
│   ├── scanner.h  
│   ├── stats.h  
│   └── worker.h  
├── copiador.c  
├── logger.c  
├── Makefile  
├── monitor.c  
├── scanner.c  
├── stats.c  
└── worker.c  

## EXplicaciones acerca del codigo
En el codigo se pueden cambiar algunos aspectos como la cantida de workers para hacer que se tenga mas de estos y halla mas estabilidad en ellos
para esto se tien que ingresar al monitor y al inicio see encuentra #define NUM_WORKERS ahi se puede editar el numero para cambiar a la cantidad deseada

Si quiere cambiar el directorio en el que se hace el back up puede cambiar esta constante #define RUTA_BACKUP en el mismo archivo, editando el nombre
entre comillas para que lo pueda leer el codigo.

Se puede ver el nombre de los archivos copiados en el archivo minisync.log, aqui se muestre junto con la hora y fecha que fueron copiados.

Para ver los archivos copiados osea el backup al momento de ejecutar se deberia crear dicha carpeta dentro del directorio donde tiene el codigo del
sincronizador con el nombre backup. Solo si no ha cambiado la ruta como se menciono antes.
## Codigos
### Para copilar el codigo ejecuta :  
make


### Para inciar el proceso de sincronizacion ingrese:  
./minysic (ruta de la carpeta a copiar)  
nota: asegurese que la carpeta no conetenga la carpeta donde esta guardado el codigo o donde se realize el backup en defecto para evitar fallos al entrar a recopiar los archivos del backup

### Para dejar de ejecutar ingrese:  
killall ./minysic  
terminara el procesos entero

