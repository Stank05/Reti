#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork(); // Funzione nativa Linux per creare un processo figlio

    if (pid < 0) {
        printf("Errore nella fork\n");
    } else if (pid == 0) {
        printf("Ciao dal processo FIGLIO! (PID Linux: %d)\n", getpid());
    } else {
        printf("Ciao dal processo PADRE! (PID Linux: %d)\n", getpid());
    }
    return 0;
}
