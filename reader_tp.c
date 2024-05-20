
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

////parto de Clase4 reader.c


#define FIFO_NAME "myfifo"

int main(void)
{
    mkfifo(FIFO_NAME, 0666);        // se tiene que crear por si el writer no empezò primero
                                    //OJO acá habrìa que preguntar por EEXIST ver 35:43
    printf("waiting for writers...\n");
    int fd = open(FIFO_NAME, O_RDONLY);
    printf("got a writer\n");

    char s[300];
    int num;
    do {
        if ((num = read(fd, s, 300)) == -1)
            perror("read");
        else {
            s[num] = '\0';
            //printf("reader: read %d bytes: \"%s\"\n", num, s);
            printf("reader: read %d bytes: %s \n",  num, s);
        }
    }
    while (num > 0);
    printf("Chau :-)\n");
}
