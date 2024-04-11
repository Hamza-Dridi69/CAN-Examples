#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>


void afficher_trame(struct can_frame trame) {
    printf("ID: 0x%03X [%d] ", trame.can_id, trame.can_dlc);
    for (int i = 0; i < trame.can_dlc; i++)
        printf("%02X ", trame.data[i]);
    printf("\n");
}

int main(int argc, char **argv){

    int s; 
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;


    //Création Socket
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket");
		return 1;
	}

    //Spécifier l'interface CAN
    strcpy(ifr.ifr_name, "vcan0" );
	ioctl(s, SIOCGIFINDEX, &ifr);

    //Liaison interface
    memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind");
		return 1;
	}

    //Préparation de la trame
    frame.can_id = 0x8123;
	frame.can_dlc = 8;
    memset(frame.data, 0xAA, 8);

    // Boucle infinie pour envoyer et recevoir des trames
    while (1) {
        // Envoyer la trame
        if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
            perror("Write");
            close(s);
            return 1;
        }

        // Recevoir et afficher les trames avec un ID entre 0x100 et 0x1FF
        while (1) {
            if (read(s, &frame, sizeof(struct can_frame)) < 0) {
                perror("Read");
                close(s);
                return 1;
            }

            // Vérifier si l'ID de la trame reçue est dans la plage spécifiée
            if ((frame.can_id >= 0x100) && (frame.can_id <= 0x1FF)) {
                afficher_trame(frame);
            }
        }
    }



    return 0;
}