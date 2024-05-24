#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

//parto del ejemplo de Clase4 writer_clase2.c

#define FIFO_NAME "myfifo"          //es una ruta relativa a la carpeta donde estamos
#define EXIT_FAILURE 1

// Prototipo del manejador de señales
void handle_siguser1(int sig);
void handle_siguser2(int sig);
void handle_sigint(int sig);

// Archivo descriptor del FIFO
int fd;


int main(void)
{
    // Declaración de estructuras utilizadas
    struct sigaction sa, sa_int, sa_user1, sa_user2;

    // Declaración de variables
    char s[300];
    int num;

    // Configurar el manejador para SIGINT
    sa_int.sa_handler = handle_sigint;
    sa_int.sa_flags = 0; // o SA_RESTART
    sigemptyset(&sa_int.sa_mask);
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // Configurar del manejador para SIGUSR1
    sa_user1.sa_handler = handle_siguser1;
    sa_user1.sa_flags = SA_RESTART;
    sigemptyset(&sa_user1.sa_mask);
    if (sigaction(SIGUSR1, &sa_user1, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    // Configurar del manejador para SIGUSR2
    sa_user2.sa_handler = handle_siguser2;
    sa_user2.sa_flags = SA_RESTART;
    sigemptyset(&sa_user2.sa_mask);
    if (sigaction(SIGUSR2, &sa_user2, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }


    // Configuración del manejador para el pipe
    // Marco a SIGPIPE para ignorar por eso SIG_IGN
    // no quiero que el proceso se muera, quiero que
    // salga por if o por else
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0; // SA_RESTART;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGPIPE, &sa, NULL) == -1){
        perror("sigaction");
        exit(1);
    };                          

    // Creación del FIFO
    //066 son los permisos, de lectura y escritura ojo que hay
    // un an not luego, es la umask 0022
    //mkfifo(FIFO_NAME, 0666);        
                            
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        if (errno == EEXIST) {
            printf("El FIFO ya existe.\n");
            //no obstante continúo el programa porque asumo fue creado por el
            //reader
        } else {
            // Mostrar mensaje de error
            perror("Error al crear el FIFO");
            exit(EXIT_FAILURE);
        }
    }
    else{
        printf("FIFO created.\n");
    }
    
    //return(0) // si quiero probar creación del archivo (inodo describe la estructura de myfifo)

    printf("writer: My PID is %d\n", getpid());
    printf("waiting for readers...\n");
    fd = open(FIFO_NAME, O_WRONLY);
    printf("got a reader--type some stuff\n");


    while (1) {
        if(fgets(s, 300, stdin) == NULL){
            break;
        }                               // stdin es el archivo de donde estamos leyendo
                                        // le agrego if == NULL para detectar EOF con ctrl D
        char d[300] = "DATA: ";         // me piden este encabezado
        if ((num = write(fd, strcat(d,s) , strlen(d) + strlen(s))) == -1){
            perror("write");
            return 1;                   //salgo porque no puede escribir mas en el pipe porque por ejemplo
                                        //murió el hijo
        }
        else
            printf("writer: wrote %d bytes\n", num);
    }
}


// Manejador para SIGINT
void handle_sigint(int sig) {
    write(1, "Ahhh! SIGINT!\n", 14);
}

// Manejador para SIGUSR1
void handle_siguser1(int sig) {
    char *msg = "SIGNAL:1\n";
    write(fd, msg, strlen(msg));
    write(1, "Ahhh! SIGUSR1!\n", 15);
}

// Manejador para SIGUSR2
void handle_siguser2(int sig) {
    char *msg = "SIGNAL:2\n";
    write(fd, msg, strlen(msg));
    write(1, "Ahhh! SIGUSR2!\n", 15);
}

// Notas: con el comando stat myfifo puedo tener datos de inodo (estructura de archivo del fifo)
// si lo ejecuto queda bloqueado en la funciòn open... no hay otro proceso que abra el FIFO 
// para leer