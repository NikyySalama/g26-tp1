#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    for (int i = 1; i < argc ; i++)
    {
        printf("Soy el slave con pid %d procesando %s \n", getpid(), argv[i]);
    }
    
    return 0;
}