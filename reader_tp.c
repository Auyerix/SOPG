
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

////parto de Clase4 reader.c


#define FIFO_NAME "myfifo"
#define EXIT_FAILURE 1

const char *file_log = "log.txt";
const char *file_sig = "sig.txt";

void append_to_file(const char *filename, const char *text);

int main(void)
{
    mkfifo(FIFO_NAME, 0666);        // se tiene que crear por si el writer no empezò primero
                                    //OJO acá habrìa que preguntar por EEXIST ver 35:43
    printf("reader: My PID is %d\n", getpid());
    printf("waiting for writers...\n");
    int fd = open(FIFO_NAME, O_RDONLY);
    printf("got a writer\n");

    char s[300];
    const char *key_string = "DATA:";
    int num;
    
    do {
        if ((num = read(fd, s, 300)) == -1)
            perror("read");
        else {
            s[num] = '\0';
            //printf("reader: read %d bytes: \"%s\"\n", num, s);
            printf("reader: read %d bytes: %s \n",  num, s);
            int resultado = strncmp(key_string, s, 5);
            
            if(resultado == 0){
                append_to_file(file_log, s);
            }
            else{
                append_to_file(file_sig, s);
            }

            
            
        }
    }
    while (num > 0);
    printf("Chau :-)\n");
}

void append_to_file(const char *filename, const char *text) {
    // Abrir el archivo en modo append
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Escribir el texto en el archivo
    if (fprintf(file, "%s", text) < 0) {
        perror("fprintf");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Cerrar el archivo
    if (fclose(file) != 0) {
        perror("fclose");
        exit(EXIT_FAILURE);
    }

    printf("Text appended to %s successfully.\n", filename);
}