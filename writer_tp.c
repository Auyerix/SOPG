#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/wait.h>

#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
//parto de Clase4 writer_clase2.c

#define FIFO_NAME "myfifo"   //es una ruta relativa a la carpeta donde estamos

// Manejador para SIGINT
void handle_sigint(int sig) {
    //printf("Received SIGINT\n");
    write(1, "Ahhh! SIGINT!\n", 14);
}

int main(void)
{
    struct sigaction sa, sa_int;

    // Configurar el manejador para SIGINT
    sa_int.sa_handler = handle_sigint;
    sa_int.sa_flags = 0; // o SA_RESTART
    sigemptyset(&sa_int.sa_mask);
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }



    // Configuración el manejador para el pipe
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0; // SA_RESTART;
    sigemptyset(&sa.sa_mask);
    
    // Acá está sigaction luego ponerles un if según PTT 3 pag 11
    sigaction(SIGPIPE, &sa, NULL);  //Marco a SIGPIPE para ignorar por eso SIG_IGN
                                    //no quiero que el proceso se muera, quiero que
                                    // salga por if o por else
    
    
    mkfifo(FIFO_NAME, 0666);  //066 son los permisos, de lectura y escritura ojo que hay
                                // un an not luego, es la umask 0022 
    //return(0) // para probar creaciòn del archivo (inodo describe la estructura de myfifo)

    printf("writer: My PID is %d\n", getpid());
    printf("waiting for readers...\n");
    int fd = open(FIFO_NAME, O_WRONLY);
    printf("got a reader--type some stuff\n");

    char s[300];
    //char d[300] = "DATA: ";

    int num;
    while (1) {
        if(fgets(s, 300, stdin) == NULL){
            break;
        }                               //stdin es el archivo de donde estamos leyendo
                                        // le agrego if == NULL para detectar EOF con ctrl D
        char d[300] = "DATA: ";         // me piden este encabezado
        if ((num = write(fd, strcat(d,s) , strlen(d) + strlen(s))) == -1){
            perror("write");
            return 1;   //salgo porque no puede escribir mas en el pipe porque por ejemplo
                        //muriò el hijo
        }
        else
            printf("writer: wrote %d bytes\n", num);
    }
}


// Notas: con el comando stat myfifo puedo tener datos de inodo (estructura de archivo del fifo)
// si lo ejecuto queda bloqueado en la funciòn open... no hay otro proceso que abra el fifo 
// para leer