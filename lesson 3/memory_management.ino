#include <Arduino.h>

#if CONFIG_FREERTOS_UNICORE
    static const BaseType_t app_cpu = 0;
#else
    static const BaseType_t app_cpu = 1;
#endif

void testTask(void *parameter) {
    while (1) {
        int a = 1;
        int b[100];  
        // This array is bigger than the 
        // stack size of the task, which is 1024 bytes.

        // 100 int values is around 400 bytes.
        // so the stack will overflow quickly
        // because it also needs 768 bytes of overhead
        // 400 + 768 = 1168 bytes, which is more than the stack size of the task.
        for (int i = 0; i < 100; i++) {
            b[i] = a+1;
        }

        Serial.println(b[0]);

        Serial.print("High water mark: ");
        Serial.println(uxTaskGetStackHighWaterMark(NULL));
    
        Serial.print("Heap before malloc: ");
        Serial.println(xPortGetFreeHeapSize());

        int *ptr = (int *)pvPortMalloc(1024* sizeof(int));
         
        if (ptr == NULL) {
            Serial.println("Not enough heap.");
        } else {
            for (int i = 0; i < 1024; i++) {
                ptr[i] = 3;
            }
        } 

        // Now it does not reset after reaching 0 heap memory left.

        Serial.print("Heap after malloc: ");
        Serial.println(xPortGetFreeHeapSize());

        // Let's add a freeing of the memory

        vPortFree(ptr);

        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void setup() {

    Serial.begin(115200);
    xTaskDelay(1000 / portTICK_PERIOD_MS);
    Serial.println();
    Serial.println("---FreeRTOS Memory Management---");

    xTaskCreatePinnedToCore(testTask,
                        "Test Task",
                        1500,
                        NULL,
                        1,
                        NULL,
                        app_cpu);

    vTaskDelete(NULL);

}

void loop() {

}