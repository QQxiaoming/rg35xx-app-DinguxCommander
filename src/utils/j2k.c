/*
 * This file is part of the https://github.com/QQxiaoming/rg35xx-app-template.git
 * project.
 *
 * Copyright (C) 2024 Quard <2014500726@smail.xtu.edu.cn>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the BSD 3-Clause License as published by
 * the Open Source Initiative; see LICENSE for more details.
 */

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

static uint keytable[] = {
    KEY_A,        // RG35XX_JOYPAD_A
    KEY_B,        // RG35XX_JOYPAD_B
    KEY_X,        // RG35XX_JOYPAD_X
    KEY_Y,        // RG35XX_JOYPAD_Y
    KEY_POWER,    // RG35XX_JOYPAD_POWER
    KEY_H,        // RG35XX_JOYPAD_L1
    KEY_L,        // RG35XX_JOYPAD_R1
    KEY_N,        // RG35XX_JOYPAD_SELECT
    KEY_M,        // RG35XX_JOYPAD_START
    KEY_U,        // RG35XX_JOYPAD_MENU
    KEY_R,        // RG35XX_JOYPAD_VOL+
    KEY_T,        // RG35XX_JOYPAD_VOL-
    KEY_J,        // RG35XX_JOYPAD_L2
    KEY_K,        // RG35XX_JOYPAD_R2
    KEY_Q,        // RG35XX_JOYPAD_Left
    KEY_D,        // RG35XX_JOYPAD_Right
    KEY_W,        // RG35XX_JOYPAD_Up
    KEY_S,        // RG35XX_JOYPAD_Down
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
                ie.code = keytable[jse.number];
                ie.value = jse.value;
                jtime2itime(jse.time,&ie.time,&ie_resend.time);
                sendkey = 1;
                bytes = write(ui_fd, &ie, sizeof(ie));
            }
        } else if(jse.type == JS_EVENT_AXIS) {
            switch(jse.number) {
                case 2:  // RG35XX_JOYPAD_L2
                    ie.code = KEY_J;
                    ie.value = (uint)(0 < jse.value);
                    jtime2itime(jse.time,&ie.time,&ie_resend.time);
                    sendkey = 1;
                    bytes = write(ui_fd, &ie, sizeof(ie));
                    break;
                case 5:  // RG35XX_JOYPAD_R2
                    ie.code = KEY_K;
                    ie.value = (uint)(0 < jse.value);
                    jtime2itime(jse.time,&ie.time,&ie_resend.time);
                    sendkey = 1;
                    bytes = write(ui_fd, &ie, sizeof(ie));
                    break;
                case 6:  // RG35XX_JOYPAD_Left/Right
                    if(jse.value != 0) uVar4 = jse.value<1?KEY_Q:KEY_D;
                    ie.code = uVar4;
                    ie.value = jse.value==0?0:1;
                    jtime2itime(jse.time,&ie.time,&ie_resend.time);
                    sendkey = 1;
                    bytes = write(ui_fd, &ie, sizeof(ie));
                    break;
                case 7:  // RG35XX_JOYPAD_Up/Down
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
            for(int i = 0; i < sizeof(keytable)/sizeof(uint); i++) {
                ioctl(ui_fd,UI_SET_KEYBIT,keytable[i]);
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
