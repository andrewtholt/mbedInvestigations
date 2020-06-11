#include "mbed.h"

#include "msg.h"

#include <map>

MemoryPool<message_t, 8> mpool;

Queue<message_t, 8> queue;

Thread ledThread;
Thread ctlThread;

#define RATE 250

void ledControlTask(void) {

    bool runFlag = true;
    DigitalOut myLed(LED1);

    while(runFlag) {
        osEvent evt = queue.get(osWaitForever);

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

    while(1) {
        message_t *messageOn = mpool.alloc();
        message_t *messageOff = mpool.alloc();

        topic = messageOn->body.hl_body.topic;
        msg = messageOn->body.hl_body.msg;

        strcpy(topic,(char *)"LED1");
        strcpy(msg,(char *)"OFF");
        queue.put(messageOn);

        ThisThread::sleep_for(RATE);

        topic = messageOff->body.hl_body.topic;
        msg = messageOff->body.hl_body.msg;

        strcpy(topic,(char *)"LED1");
        strcpy(msg,(char *)"ON");
        queue.put(messageOff);

        ThisThread::sleep_for(RATE);
    }
}


int main() {

    ledThread.start(ledControlTask);
    ctlThread.start(control);

    while(1) {
        Thread::wait(5);
    }
}




