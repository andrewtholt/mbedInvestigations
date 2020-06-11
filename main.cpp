#include "mbed.h"

#include "msg.h"
#include "tasks.h"

#include <map>

Queue<message_t, 8> tasks[(int)taskId::LAST];
MemoryPool<message_t, 8> mpool;

Thread ledThread;
Thread ctlThread;

#define RATE 250

void ledControlTask(void) {

    int iam = (int) taskId::LED_CTRL;
    Queue<message_t, 8> myQueue = tasks[iam];

    bool runFlag = true;
    DigitalOut myLed(LED1);

    while(runFlag) {
        osEvent evt = tasks[iam].get(osWaitForever);

        if (evt.status == osEventMessage ) {
            message_t *message = (message_t*)evt.value.p;

            char *topic = message->body.hl_body.topic;

            if(!strcmp(topic,"LED1")) {
                char *msg = message->body.hl_body.msg;
                if(!strcmp(msg,"ON")) {
                    myLed=1;
                } else if(!strcmp(msg,"OFF")) {
                    myLed=0;
                }
            }

            mpool.free(message);
        }
    }
}

void control() {

    char *topic;
    char *msg;

    int iam = (int) taskId::CTRL;
    int dest = (int) taskId::LED_CTRL;

    while(1) {
        message_t *messageOn = mpool.alloc();
        message_t *messageOff = mpool.alloc();

        topic = messageOn->body.hl_body.topic;
        msg = messageOn->body.hl_body.msg;

        strcpy(topic,(char *)"LED1");
        strcpy(msg,(char *)"OFF");
        tasks[dest].put(messageOn);

        ThisThread::sleep_for(RATE);

        topic = messageOff->body.hl_body.topic;
        msg = messageOff->body.hl_body.msg;

        strcpy(topic,(char *)"LED1");
        strcpy(msg,(char *)"ON");
        tasks[dest].put(messageOff);

        ThisThread::sleep_for(RATE);
    }
}


int main() {

    ledThread.start(ledControlTask);
    ctlThread.start(control);

    ledThread.join();
    ctlThread.join();
/*
    while(1) {
//        Thread::wait(5);
        ThisThread::sleep_for(RATE);
    }
    ledThread.join();
    ctlThread.join();
    */
}




