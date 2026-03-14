#include <Arduino.h>

#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

static const int led_pin = 2; // GPIO pin for the LED
static const int change_pin = 0; // GPIO pin for the button

static QueueHandle_t blinkQueue = NULL;

/*
IRAM_ATTR - signal the compiler to 
place the function in the internal RAM 
of the microcontroller, which allows for faster execution 
and is essential for interrupt service routines (ISRs) 
to ensure they run efficiently without being affected by 
external factors like flash memory access times.
*/

void IRAM_ATTR handleButtonPress() {
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();

    if (interrupt_time - last_interrupt_time > 200) {
        static bool fast = false;
        fast = !fast;
        int delayTime = fast ? 250 : 1000; // Toggle between 250ms and 1000ms

        xQueueOverwriteFromISR(
            blinkQueue,
            &delayTime, 
            NULL
        ); // Send the new delay time to the queue
        last_interrupt_time = interrupt_time;
    }
}

void ledTask(void *paramater) {
  int currentDelay = 1000; // Start with slow blinking
  int newDelay;

  while(1) {
    if (xQueueReceive(blinkQueue, &newDelay, 0) == pdPASS) {
        currentDelay = newDelay; // Update the delay time based on the queue
    }

    digitalWrite(led_pin, !digitalRead(led_pin));
    vTaskDelay(currentDelay / portTICK_PERIOD_MS); // Delay based on the current interval}
}};


void setup() {
    pinMode(led_pin, OUTPUT);
    pinMode(change_pin, INPUT_PULLDOWN);

    blinkQueue = xQueueCreate(1, sizeof(int)); // Create a queue to hold the delay time

    attachInterrupt(digitalPinToInterrupt(change_pin), handleButtonPress, FALLING); // Attach interrupt to the button pin
    xTaskCreatePinnedToCore(
        ledTask,        // Function that implements the task.
        "LED Task",     // Text name for the task.
        2048,           // Stack size in words, not bytes.
        NULL,           // Parameter passed into the task.
        1,              // Priority at which the task is created.
        NULL,           // Pointer to the task handle.
        app_cpu         // Core where the task should run.
    );
    
    vTaskDelete(NULL); // Delete the loop task to free up resources.
};

void loop() {

};