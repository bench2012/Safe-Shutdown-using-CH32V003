
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

// Set up the button pin and the system poweroff command
#define BUTTON_PIN "2"
// const char *POWER_OFF_COMMAND[] = {"sudo", "poweroff", NULL};

// Define the button press callback function
void button_callback(void) {
    printf("Button pressed, powering off system...\n");
    system("poweroff");
}

int main(void) {
    int fd, len;
    char buf[2];
    struct pollfd pfd;

    // Export the button pin
    fd = open("/sys/class/gpio/export", O_WRONLY);
    len = snprintf(buf, sizeof(buf), "%s", BUTTON_PIN);
    write(fd, buf, len);
    close(fd);

    // Set the button pin direction and edge
    fd = open("/sys/class/gpio/gpio2/direction", O_WRONLY);
    write(fd, "in", 2);
    close(fd);
    fd = open("/sys/class/gpio/gpio2/edge", O_WRONLY);
    write(fd, "rising", 6);
    close(fd);

    // Open the button value file descriptor for polling
    fd = open("/sys/class/gpio/gpio2/value", O_RDONLY);

    // Set up the poll structure for the button file descriptor
    pfd.fd = fd;
    pfd.events = POLLPRI;

    // Wait for button press
    printf("Waiting for button press...\n");
    int first_interrupt = 0;
    for (;;) {
        poll(&pfd, 1, -1);
        lseek(fd, 0, SEEK_SET);
        read(fd, buf, 2);
        if (first_interrupt == 0) {
            first_interrupt = 1;
        } else {
            button_callback();
        }
    }

    // Unexport the button pin on exit
    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    len = snprintf(buf, sizeof(buf), "%s", BUTTON_PIN);
    write(fd, buf, len);
    close(fd);

    return 0;
}
