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

void lecture_semantics(int s, struct can_frame *frame_sem, int num_iterations) {
    struct can_filter rfilter[1];
    int nbytes;

    for (int i = 0; i < num_iterations; i++) {
        rfilter[0].can_id   = 0xC00 + i;
        rfilter[0].can_mask = 0xFFF;

        setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

        nbytes = read(s, &frame_sem[i], sizeof(struct can_frame));
        
        if (nbytes < 0) {
            perror("Read");
            exit(1);
        }
        afficher_trame(frame_sem[i]);
    }
}

void write_frame(int s, struct can_frame frame){

    if (write(s, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
            perror("Write");
            close(s);
            exit(1);
    }

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

    lecture_semantics(s, frame_sem, MAX_FRAMES);
    /*
    while(1) {
        
   
        lecture_semantics(s, frame_sem, 6);
        

        for (c=0; c<6; c++)
            printf("angle : %i, road : %02x", c, frame_sem[c].data[0]);
    


        
        

        // Utiliser les trames stockées dans frame_sem comme nécessaire
        
    }

    */
    
    frame.can_id = 0x123;
	frame.can_dlc = 2;
    frame.data[0]=0x00;
    frame.data[1]=0x01;

    write_frame(s,frame);

    sleep(2);

    frame.can_id = 0x123;
	frame.can_dlc = 2;
    frame.data[0]=0x00;
    frame.data[1]=0x10;

    write_frame(s,frame);
    
    sleep(2);

    frame.can_id = 0x123;
	frame.can_dlc = 2;
    frame.data[0]=0x01;
    frame.data[1]=0x00;

    write_frame(s,frame);
    
    sleep(2);

    frame.can_id = 0x123;
	frame.can_dlc = 2;
    frame.data[0]=0x00;
    frame.data[1]=0x00;

    write_frame(s,frame);

    frame.can_id = 0x321;
	frame.can_dlc = 3;
    frame.data[0]=0x64;
    frame.data[1]=0x00;
    frame.data[2]=0x05;


    write_frame(s,frame);

    sleep(4.8);


    frame.can_id = 0x321;
	frame.can_dlc = 3;
    frame.data[0]=0x64;
    frame.data[1]=0x00;
    frame.data[2]=0x08;


    write_frame(s,frame);

    sleep(5);

    frame.can_id = 0x321;
	frame.can_dlc = 3;
    frame.data[0]=0x64;
    frame.data[1]=0x00;
    frame.data[2]=0x00;


    write_frame(s,frame);

    sleep(2);


    frame.can_id = 0x321;
	frame.can_dlc = 3;
    frame.data[0]=0x00;
    frame.data[1]=0x64;
    frame.data[2]=0x00;


    write_frame(s,frame);
    

    

    

    

    

    return 0;
}
