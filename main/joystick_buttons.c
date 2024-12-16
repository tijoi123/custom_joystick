#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "joystick_buttons.h"
#include "button.h"

#define JOYSTICK_BUTTONS_TAG "HID_JOYSTICK_BUTTONS"

#define BUTTON_SELECT  (PIN_BIT(BUTTON_1) | \
    PIN_BIT(BUTTON_2) | \
    PIN_BIT(BUTTON_3) | \
    PIN_BIT(BUTTON_4) | \
    PIN_BIT(BUTTON_5) | \
    PIN_BIT(BUTTON_6) | \
    PIN_BIT(BUTTON_7) | \
    PIN_BIT(BUTTON_8) | \
    PIN_BIT(BUTTON_9) | \
    PIN_BIT(BUTTON_10) | \
    PIN_BIT(BUTTON_11) | \
    PIN_BIT(BUTTON_12) | \
    PIN_BIT(BUTTON_13) | \
    PIN_BIT(BUTTON_14) | \
    PIN_BIT(BUTTON_15) | \
    PIN_BIT(BUTTON_16) | \
    PIN_BIT(BUTTON_17) | \
    PIN_BIT(BUTTON_18) | \
    PIN_BIT(BUTTON_19) | \
    PIN_BIT(BUTTON_20))

static const button_t button_idx_map[] = {
    [0]{BUTTON_1},
    [1]{BUTTON_2},
    [2]{BUTTON_3},
    [3]{BUTTON_4},
    [4]{BUTTON_5},
    [5]{BUTTON_6},
    [6]{BUTTON_7},
    [7]{BUTTON_8},
    [8]{BUTTON_9},
    [9]{BUTTON_10},
    [10]{BUTTON_11},
    [11]{BUTTON_12},
    [12]{BUTTON_13},
    [13]{BUTTON_14},
    [14]{BUTTON_15},
    [15]{BUTTON_16},
    [16]{BUTTON_17},
    [17]{BUTTON_18},
    [18]{BUTTON_19},
    [19]{BUTTON_20},
};





static const size_t button_idx_map_length = sizeof(button_idx_map) / sizeof(button_t);

static QueueHandle_t * queue = NULL;

static void send_event(uint32_t state) {
    joystick_buttons_event_t event = {
        .state = state,
    };
    xQueueSend(queue, &event, 1000/portTICK_PERIOD_MS);
}

static void joystick_buttons_task(void *pvParameter)
{


    button_event_t ev;
    QueueHandle_t button_events = button_Init(BUTTON_SELECT);
    uint32_t button_state = 0;

    while (true) {
        if (xQueueReceive(button_events, &ev, 1000/portTICK_PERIOD_MS)) {
            for (int idx=0; idx < button_idx_map_length; idx++) {
                if (button_idx_map[idx].pin == ev.pin) {
                    // ESP_LOGI(JOYSTICK_BUTTONS_TAG, "button pin: %d event: %d idx: %d", ev.pin, ev.event, idx);

                    if (ev.event == BUTTON_UP) {
                        button_state &= ~(1<<idx);
                    } else {
                        button_state |= (1<<idx);
                    }
                }
            }

            send_event(button_state);
        }
        // ESP_LOGI(JOYSTICK_BUTTONS_TAG, "tick %d", button_state);
    }
}

QueueHandle_t * joystick_buttons_init(void)
{
    queue = xQueueCreate(4, sizeof(joystick_buttons_event_t));

    xTaskCreate(joystick_buttons_task, "button_task", 4096, NULL, 4, NULL);

    return queue;
}
