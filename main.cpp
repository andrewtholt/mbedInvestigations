#include "mbed.h"

#include "msg.h"

#include <map>

MemoryPool<hl_message_t, 8> mpool;
Queue<hl_message_t, 8> queue;

Thread ledThread;
Thread ctlThread;

#define RATE 250

void ledControlTask(void) {

    bool runFlag = true;
    DigitalOut myLed(LED1);

    while(runFlag) {
        osEvent evt = queue.get(osWaitForever);

        if (evt.status == osEventMessage ) {
            hl_message_t *message = (hl_message_t*)evt.value.p;

            if(!strcmp(message->topic,"LED1")) {
                if(!strcmp(message->msg,"ON")) {
                    myLed=1;
                } else if(!strcmp(message->msg,"OFF")) {
                    myLed=0;
                }
            }

            mpool.free(message);
        }
    }
}

void control() {

    while(1) {
        hl_message_t *messageOn = mpool.alloc();
        hl_message_t *messageOff = mpool.alloc();

        strcpy(messageOn->topic,(char *)"LED1");
        strcpy(messageOn->msg,(char *)"OFF");
        queue.put(messageOn);

        ThisThread::sleep_for(RATE);

        strcpy(messageOff->topic,(char *)"LED1");
        strcpy(messageOff->msg,(char *)"ON");
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




