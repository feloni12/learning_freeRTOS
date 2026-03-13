#include <Arduino.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Pins

static const int ledPin = LED_BUILTIN;

// Our task: blink the LED every second
void toggleLed(void *parameter) {
    while(1) {
        digitalWrite(ledPin, HIGH);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // vTaskDelay is a non-blocking delay function
        digitalWrite(ledPin, LOW);
        vTaskDelay(1000 / portTICK_PERIOD_MS); // it expects a delay in ticks, so we convert milliseconds to ticks using portTICK_PERIOD_MS
    }
}

void toggleLedDiffInterval(void *parameter) {
    while(1) {
        digitalWrite(ledPin, HIGH);
        vTaskDelay(500 / portTICK_PERIOD_MS); // Toggle every 500ms
        digitalWrite(ledPin, LOW);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void setup() {
    pinMode(ledPin, OUTPUT);

    xTaskCreatePinnedToCore(
        toggleLed,      // Function that implements the task.
        "Toggle LED",   // Text name for the task.
        1024,           // Stack size in words, not bytes.
        NULL,           // Parameter passed into the task.
        1,              // Priority at which the task is created.
        NULL,           // Pointer to the task handle.
        app_cpu         // Core where the task should run.
    );

    xTaskCreatePinnedToCore(
        toggleLedDiffInterval,      // Function that implements the task.
        "Toggle LED Diff Interval",   // Text name for the task.
        1024,           // Stack size in words, not bytes.
        NULL,           // Parameter passed into the task.
        2,              // Priority at which the task is created. (Higher priority)
        NULL,           // Pointer to the task handle.
        app_cpu         // Core where the task should run.
    )
    

}

void loop() {

}