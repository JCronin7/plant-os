#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include <stdint.h>
#include <stdbool.h>
#include <FreeRTOS_SAMD21.h>
#include <queue.h>

typedef struct transport_payload
{
    uint8_t id;
    uint8_t pl_size;
    uint8_t payload[32];
} transport_payload_t;

class Transport
{
private:
    QueueHandle_t request;
    QueueHandle_t response;

public:
    Transport(uint16_t buffer_size);

    uint8_t request_send(transport_payload *payload);

    uint8_t request_receive(transport_payload *payload);

    uint8_t response_send(transport_payload *payload);

    uint8_t response_receive(uint8_t sender_id,
                             transport_payload *payload);
};

#endif /* _TRANSPORT_H_ */