#include <Arduino.h>

#if CONFIG_FREERTOS_UNICORE
    static const BaseType_t app_cpu = 0;
#else
    static const BaseType_t app_cpu = 1;
#endif

volatile bool notifyTaskB = false;
static char *ptr = NULL;

// Pass a message

// Task A:
// - Listens for input from serial monitor
// - On newline char ('/n'), stores all chars up to that point in heap memory
// - Notifies Task B of new message

// Task B:
// - Waits for notification from Task A
// - Prints message found in heap memory to Serial Monitor
// - Frees heap memory

void taskA(void *parameter) {
    unsigned char bytes='0';
    while(1) {
         if (Serial.available() > 0) {
            ptr = (char *)pvPortMalloc(100 * sizeof(char));
            if (ptr != NULL) {
                strcpy(ptr, message.c_str());
                notifyTaskB = true;
            }
        }
    }
}

void taskB(void *parameter) {
    while (1) {
        if (notifyTaskB) {
            Serial.println((char *)ptr);
            vPortFree(ptr);
            notifyTaskB = false;
        }
    }
}


void setup() {

    Serial.begin(115200);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    //Serial.println();
    //Serial.println("---FreeRTOS Memory Management---");

    xTaskCreatePinnedToCore(taskA,
                        "Task A",
                        1024,
                        NULL,
                        1,
                        &taskAHandle,
                        app_cpu);
    
    xTaskCreatePinnedToCore(taskB,
                        "Task B",
                        1024,
                        NULL,
                        1,
                        &taskBHandle,
                        app_cpu);

    vTaskDelete(NULL);

}

void loop() {

}