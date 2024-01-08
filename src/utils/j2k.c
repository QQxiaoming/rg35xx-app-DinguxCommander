// Compile j2k.c into j2k.so and load it through 
// LD_PRELOAD to help other programs use joypad to map to the keyboard

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/uinput.h>
#include <linux/joystick.h>

typedef struct keypair_s {
    int key;
    char joy;
} keypair;

/*
 SDLK_w            // Up
 SDLK_d            // Right
 SDLK_s            // Down
 SDLK_q            // Left
 SDLK_y            // Y
 SDLK_h            // L
 SDLK_l            // R
 SDLK_a            // A
 SDLK_b            // B
 SDLK_x            // X
 SDLK_n            // SELEC
 SDLK_m            // START
*/
static keypair keytable[] = {
    {KEY_A, 0},
    {KEY_B, 1},
    {KEY_X, 2},
    {KEY_Y, 3},
    {KEY_POWER, 4},
    {KEY_H, 5},
    {KEY_L, 6},
    {KEY_N, 7},
    {KEY_M, 8},
    {KEY_U, 9},
    {KEY_R, 10},
    {KEY_T, 11},
    {KEY_J, 12},
    {KEY_K, 13},
    {KEY_Q, 14},
    {KEY_D, 15},
    {KEY_W, 16},
    {KEY_S, 17},
};
static pthread_t j2k_pt = 0;
static int js_fd = -1, ui_fd = -1;

static void jtime2itime(uint jtime,struct timeval *itime1, struct timeval *itime2) {
    itime1->tv_sec = jtime / 1000;
    itime1->tv_usec = (jtime % 1000) * 1000;
    itime2->tv_sec = jtime / 1000;
    itime2->tv_usec = (jtime % 1000) * 1000;
}

static void *j2k_thread(void* arg) {
    ssize_t bytes; 
    struct js_event jse;
    struct pollfd pfd = {
        .fd = js_fd,
        .events = POLLIN,
        .revents = 0,
    };
    struct input_event ie = {
        .type = EV_KEY,
    };
    struct input_event ie_resend = {
        .type = 0,
        .code = 0,
        .value = 0,
    };
    ushort uVar4 = 0;
    ushort uVar5 = 0;
    int sendkey = 0;
    while(1) {
        bytes = read(js_fd, &jse, sizeof(jse));
        if(jse.type == JS_EVENT_BUTTON) {
            if(jse.number < 12) {
                ie.code = keytable[jse.number].key;
                ie.value = jse.value;
                jtime2itime(jse.time,&ie.time,&ie_resend.time);
                sendkey = 1;
                bytes = write(ui_fd, &ie, sizeof(ie));
            }
        } else if(jse.type == JS_EVENT_AXIS) {
            switch(jse.number) {
                case 2:
                    ie.code = KEY_J;
                    ie.value = (uint)(0 < jse.value);
                    jtime2itime(jse.time,&ie.time,&ie_resend.time);
                    sendkey = 1;
                    bytes = write(ui_fd, &ie, sizeof(ie));
                    break;
                case 5:
                    ie.code = KEY_K;
                    ie.value = (uint)(0 < jse.value);
                    jtime2itime(jse.time,&ie.time,&ie_resend.time);
                    sendkey = 1;
                    bytes = write(ui_fd, &ie, sizeof(ie));
                    break;
                case 6:
                    if(jse.value != 0) uVar4 = jse.value<1?KEY_Q:KEY_D;
                    ie.code = uVar4;
                    ie.value = jse.value==0?0:1;
                    jtime2itime(jse.time,&ie.time,&ie_resend.time);
                    sendkey = 1;
                    bytes = write(ui_fd, &ie, sizeof(ie));
                    break;
                case 7:
                    if(jse.value != 0) uVar5 = jse.value<1?KEY_W:KEY_S;
                    ie.code = uVar5;
                    ie.value = jse.value==0?0:1;
                    jtime2itime(jse.time,&ie.time,&ie_resend.time);
                    sendkey = 1;
                    bytes = write(ui_fd, &ie, sizeof(ie));
                    break;
		        default:
                    break;
            }
        }
        if(sendkey) {
            if (poll(&pfd, 1, 0) == 0) {
                bytes = write(ui_fd, &ie_resend, sizeof(ie_resend));
                sendkey = 0;
            }
        }
    }
    (void)bytes;
    return NULL;
}

void __attribute__((constructor)) init() {
    ssize_t bytes; 
    int ret = -1;
    js_fd = open("/dev/input/js0", O_RDONLY);
    if(-1 < js_fd) {
        ui_fd = open("/dev/uinput",O_WRONLY|O_NONBLOCK);
        if (-1 < ui_fd) {
            ioctl(ui_fd,UI_SET_EVBIT,1);
            for(int i = 0; i < sizeof(keytable)/sizeof(keypair); i++) {
                ioctl(ui_fd,UI_SET_KEYBIT,keytable[i].key);
            }
            struct uinput_user_dev uud;
            memset(&uud, 0, sizeof(uud));
            snprintf(uud.name, UINPUT_MAX_NAME_SIZE, "j2k device");
            bytes = write(ui_fd, &uud, sizeof(uud));
            ret = ioctl(ui_fd,UI_DEV_CREATE);
            if(ret < 0){
                fprintf(stderr,"DEV_CREATE failed %s\n",strerror(ret));
            } else {
                pthread_create(&j2k_pt, NULL, j2k_thread, NULL);
            }
        }
    }
    (void)bytes;
}

void __attribute__((destructor)) finit() {
    if (j2k_pt != 0) {
        pthread_cancel(j2k_pt);
        pthread_join(j2k_pt,NULL);
    }
    if(-1 < js_fd) {
        close(js_fd);
    }
    if(-1 < ui_fd) {
        close(js_fd);
    }
}

int main(void) {
    if(j2k_pt == 0) {
        return 1;
    }
    
    while(1) {
        pause();
    }
}
