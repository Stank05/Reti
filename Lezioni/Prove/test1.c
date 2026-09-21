#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 9999
#define BUFFER_SIZE 64

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("Errore fork");
        return 1;
    }

    // --- PROCESSO FIGLIO: CLIENT ---
    if (pid == 0) {
        sleep(1); // Attende un istante che il server vada in listen

        int sock = socket(AF_INET, SOCK_STREAM, 0);
        struct sockaddr_in serv_addr;
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);
        inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            perror("[Client] Connessione fallita");
            close(sock);
            exit(1);
        }

        const char *msg = "PING da client";
        write(sock, msg, strlen(msg));
        printf("[Client] Messaggio inviato: \"%s\"\n", msg);

        close(sock);
        exit(0);
    }

    // --- PROCESSO PADRE: SERVER ---
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    // Permette di riutilizzare subito la porta senza attendere il timeout TIME_WAIT
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 1);
    printf("[Server] In attesa su porta %d...\n", PORT);

    int client_fd = accept(server_fd, NULL, NULL);
    char buffer[BUFFER_SIZE] = {0};
    ssize_t bytes = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("[Server] Ricevuto con successo: \"%s\"\n", buffer);
    }

    close(client_fd);
    close(server_fd);

    // Attende la terminazione del processo figlio per evitare processi zombie
    wait(NULL);
    printf("[Test] Connessione completata con successo.\n");

    return 0;
}
