#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>


#define MAX_FRAMES 8

// Constantes pour les plages
#define FULL_LEFT_MIN 0x9C
#define FULL_LEFT_MAX 0x80

#define LEFT_MIN 0x80
#define LEFT_MAX 0xB0

#define MIDDLE_LEFT_MIN 0xB0
#define MIDDLE_LEFT_MAX 0xE0

#define MIDDLE_RIGHT_MIN 0xE0
#define MIDDLE_RIGHT_MAX 0x10

#define RIGHT_MIN 0x10
#define RIGHT_MAX 0x64

#define FULL_RIGHT_MIN 0x64
#define FULL_RIGHT_MAX 0x9A



void afficher_trame(struct can_frame trame) {
    printf("ID: 0x%03X [%d] ", trame.can_id, trame.can_dlc);
    for (int i = 0; i < trame.can_dlc; i++)
        printf("%02X ", trame.data[i]);
    printf("\n");
}

void afficher_vitesse_gear(struct can_frame frame) {
    int speed = frame.data[0];
    int gear = frame.data[1];

    printf("Speed: %d km/h\n", speed);
    printf("Gear: %d\n", gear);
}

void lecture_vitesse(int s, struct can_frame *frame_sem, int num_iterations) {
    struct can_filter rfilter[1];
    int nbytes;

    
    rfilter[0].can_id   = 0xC07;
    rfilter[0].can_mask = 0xFFF;

    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

    nbytes = read(s, frame_sem, sizeof(struct can_frame));
    
    if (nbytes < 0) {
        perror("Read");
        exit(1);
    }
    afficher_vitesse_gear(*frame_sem);
    
}




int main(int argc, char **argv){
    int s; 
    struct sockaddr_can addr;
    struct can_frame frame;
    struct can_frame frame_sem[MAX_FRAMES];
    struct ifreq ifr;
    int c;
    int z = 0;
    

    // Création Socket
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket");
        return 1;
    }

    // Spécifier l'interface CAN
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(s, SIOCGIFINDEX, &ifr);

    // Liaison interface
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind");
        return 1;
    }

    lecture_vitesse(s, &frame, MAX_FRAMES);
    
    while(1) {
        
   
        lecture_vitesse(s, &frame, 6);
        


        
        

        // Utiliser les trames stockées dans frame_sem comme nécessaire
        
    }

    
    
    

    

    

    

    

    return 0;
}
