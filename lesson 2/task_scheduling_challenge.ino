#include <Arduino.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = 2; // GPIO pin for the LED
static const int change_pin = 0; // GPIO pin for the button

static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;

enum blinkInterval {
    FAST_BLINK,
    SLOW_BLINK,
    INTERVAL_NUMBERS
};

blinkInterval currentInterval = SLOW_BLINK;


//Task: print msg length to serial terminal (lower priority)
void toggleLed(void *paramater) {
    while(1) {
        digitalWrite(led_pin, HIGH);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // vTaskDelay is a non-blocking delay function
        digitalWrite(led_pin, LOW);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // it expects a delay in ticks, so we convert milliseconds to ticks using portTICK_PERIOD_MS
    }
}

void toggleLedDiffInterval(void *paramater) {
    while(1) {
        digitalWrite(led_pin, HIGH);
        vTaskDelay(250 / portTICK_PERIOD_MS); // Toggle every 500ms
        digitalWrite(led_pin, LOW);
        vTaskDelay(250 / portTICK_PERIOD_MS);
    }
}



void setup() {
    pinMode(led_pin, OUTPUT);
    pinMode(change_pin, INPUT_PULLDOWN);

    xTaskCreatePinnedToCore(
        toggleLed,     // Function that implements the task.
        "Led Interval 1",       // Text name for the task.
        1024,           // Stack size in words, not bytes.
        NULL,           // Parameter passed into the task.
        1,              // Priority at which the task is created. (Lower priority)
        &task_1,        // Pointer to the task handle.
        app_cpu         // Core where the task should run.
    );

    xTaskCreatePinnedToCore(
        toggleLedDiffInterval,     // Function that implements the task.
        "Led Interval 2",       // Text name for the task.
        1024,           // Stack size in words, not bytes.
        NULL,           // Parameter passed into the task.
        1,              // Priority at which the task is created. (Higher priority)
        &task_2,        // Pointer to the task handle.
        app_cpu         // Core where the task should run.
    );

    vTaskSuspend(task_2); // Start with task_2 suspended
}

void loop() {
    // Check if the button is pressed
    if (digitalRead(change_pin) == LOW) {
        vTaskDelay(50 / portTICK_PERIOD_MS);

        currentInterval = (blinkInterval)((currentInterval + 1) % INTERVAL_NUMBERS); // Cycle through intervals
        if (currentInterval == FAST_BLINK) {
            vTaskResume(task_2); // Resume task_2 for fast blinking
            vTaskSuspend(task_1); // Suspend task_1 for fast blinking
        } else {
            vTaskSuspend(task_2); // Suspend task_2 for slow blinking
            vTaskResume(task_1); // Resume task_1 for slow blinking
        }

        while(digitalRead(change_pin) == LOW) {
            vTaskDelay(10 / portTICK_PERIOD_MS); // Wait for button release
        }
    }
}