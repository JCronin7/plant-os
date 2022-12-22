#include <transport.h>

#define WAIT_TICKS 1

Transport::Transport(uint16_t buffer_size)
{
    request  = xQueueCreate(buffer_size, sizeof(transport_payload_t));
    response = xQueueCreate(buffer_size, sizeof(transport_payload_t));
}

uint8_t Transport::request_send(transport_payload *payload)
{
    return xQueueSendToBack(request, payload, (TickType_t)WAIT_TICKS) ? payload->pl_size : 0;
}

uint8_t Transport::request_receive(transport_payload *payload)
{
    payload->pl_size = 0;
    return xQueueReceive(request, payload, (TickType_t)WAIT_TICKS) ? payload->pl_size : 0;
}

uint8_t Transport::response_send(transport_payload *payload)
{
    return xQueueSendToBack(response, payload, (TickType_t)WAIT_TICKS) ? payload->pl_size : 0;
}

uint8_t Transport::response_receive(uint8_t sender_id,
                                    transport_payload *payload)
{
    payload->pl_size = 0;

    if (xQueuePeek(response, payload, (TickType_t)WAIT_TICKS))
    {
        if (payload->id != sender_id ||
            !xQueueReceive(response, payload, (TickType_t)WAIT_TICKS))
        {
            memset(payload, 0, payload->pl_size);
        }
    }

    return payload->pl_size;
}
