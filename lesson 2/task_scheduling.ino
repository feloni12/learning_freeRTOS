#include <Arduino.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

const char msg[] = "Hello from Task!";

static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;

//Task: print msg length to serial terminal (lower priority)
void startTask1(void *paramater) {

    int msg_len = strlen(msg);

    while (1) {
        Serial.println();
        for (int i = 0; i<msg_len; i++) {
            Serial.print(msg[i]);
        }
        Serial.println();
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1 second
    }
}

void startTask2(void *parameter) {
    while (1) {
        Serial.print("*");
        vTaskDelay(100/ portTICK_PERIOD_MS); // Delay for 100ms
    }
}

void setup() {
    Serial.begin(300);

    vTaskDelay(1000 / portTICK_PERIOD_MS); // Delay for 1 seconds to allow serial monitor to initialize
    Serial.print("Setup and loop task running on core ");
    Serial.println(xPortGetCoreID());
    Serial.print("with priority ");
    Serial.println(uxTaskPriorityGet(NULL)); // Get the priority of the current task (

    xTaskCreatePinnedToCore(
        startTask1,     // Function that implements the task.
        "Task 1",       // Text name for the task.
        1024,           // Stack size in words, not bytes.
        NULL,           // Parameter passed into the task.
        1,              // Priority at which the task is created. (Lower priority)
        &task_1,        // Pointer to the task handle.
        app_cpu         // Core where the task should run.
    );

    xTaskCreatePinnedToCore(
        startTask2,     // Function that implements the task.
        "Task 2",       // Text name for the task.
        1024,           // Stack size in words, not bytes.
        NULL,           // Parameter passed into the task.
        2,              // Priority at which the task is created. (Higher priority)
        &task_2,        // Pointer to the task handle.
        app_cpu         // Core where the task should run.
    );
}

void loop() {
    for(int i = 0; i < 3; i++) {
        vTaskSuspend(task_2);
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay for 2 seconds
        vTaskResume(task_2);
        vTaskDelay(2000 / portTICK_PERIOD_MS); // Delay for 2
    }

    if (task_1 != NULL) {
        vTaskDelete(task_1);
        task_1 = NULL;
    }
}