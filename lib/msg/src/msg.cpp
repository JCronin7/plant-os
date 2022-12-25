/**
 * @file msg.cpp
 * @author Jake Cronin (jcronin97@live.ca)
 * @brief
 *  Provides an interface to send asynchronous messages between multiple
 *  requestors (clients) and a single server.
 * @version 0.1
 * @date 2022-12-22
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <msg.h>

#define WAIT_TICKS 0
#define SEND(queue, pl_ptr) \
    (xQueueSendToBack((queue), (pl_ptr), (TickType_t)WAIT_TICKS) ? (pl_ptr)->pl_size : 0)

/**
 * @brief
 *  Payload structure for message queues
 *
 */
struct Payload
{
    /* Id of the module sending the payload */
    uint8_t id;
    /* Active Size of the payload */
    uint8_t pl_size;
    /* Payload buffer */
    uint8_t payload[MSG_MAX_PL_LENGTH];
};

/**
 * @brief
 *  Construct an empty Pipe object
 *
 */
Msg::Pipe::Pipe(void) : request(nullptr), response(nullptr) { }

/**
 * @brief
 *  Construct a new Pipe object
 *
 * @param[in] buffer_size - Elements in each queue
 */
Msg::Pipe::Pipe(uint16_t buffer_size)
{
    request  = xQueueCreate(buffer_size, sizeof(Payload));
    response = xQueueCreate(buffer_size, sizeof(Payload));
}

/**
 * @brief
 *  Construct a new Client object
 *
 */
Msg::Client::Client(void) : pipe(nullptr) { }

/**
 * @brief
 *  Construct a new Client object
 *
 * @param[in] pipe - Queues shared by server/client
 */
Msg::Client::Client(Pipe *pipe) : pipe(pipe) { }

/**
 * @brief
 *  Set client ID
 *
 * @param[in] id - ID byte
 */
void Msg::Client::idSet(uint8_t id)
{
    this->id = id;
}

/**
 * @brief
 *  Send a request to a Server
 *
 * @param[in] pl_ptr - Pointer to data to send
 * @param[in] size   - Number of bytes to send
 *
 * @return
 *  Bytes sent
 */
uint8_t Msg::Client::send(void *pl_ptr, uint8_t size)
{
    if (pipe == nullptr)
    {
        return 0;
    }

    Payload payload = {
        .id = this->id,
        .pl_size = size,
    };
    memcpy((void*)&(payload.payload), pl_ptr, size);

    const uint8_t sent = SEND(pipe->request, &payload);
    vTaskResume(pipe->server);
    return sent;
}

/**
 * @brief
 *  Receive response from server
 *
 * @param[out] pl_ptr - Pointer to data to send
 *
 * @return
 *  Bytes received
 */
uint8_t Msg::Client::receive(void *pl_ptr)
{
    //memset(pl_ptr, 0, MSG_MAX_PL_LENGTH);

    if (pipe == nullptr)
    {
        return 0;
    }

    Payload payload;

    if (!xQueuePeek(pipe->response, &payload, (TickType_t)WAIT_TICKS))
    {
        return 0;
    }

    if (payload.id != this->id ||
        !xQueueReceive(pipe->response, &payload, (TickType_t)WAIT_TICKS))
    {
        memset(&payload, 0, payload.pl_size);
    }

    memcpy(pl_ptr, (void*)&(payload.payload), payload.pl_size);
    return payload.pl_size;
}

/**
 * @brief Construct a new Server object
 *
 */
Msg::Server::Server(void) : pipe(nullptr) { }

/**
 * @brief Construct a new Server object
 *
 * @param[in] pipe - Queues shared by server/client
 */
Msg::Server::Server(Pipe *pipe) : pipe(pipe) { }

/**
 * @brief
 *  Send a response to the client that sent the request
 *
 * @param[in] pl_ptr - Pointer to data to send
 * @param[in] size   - Number of bytes to send
 *
 * @return
 *  Number of bytes sent
 */
uint8_t Msg::Server::send(void *pl_ptr, uint8_t size)
{
    if (pipe == nullptr)
    {
        return 0;
    }

    Payload payload = {
        .id = last_id,
        .pl_size = size,
    };
    memcpy((void*)&(payload.payload), pl_ptr, size);
    return SEND(pipe->response, &payload);
}

/**
 * @brief
 *  Receive a request from a client
 *
 * @param[out] pl_ptr - Pointer to received data
 *
 * @return
 *  Number of bytes received
 */
uint8_t Msg::Server::receive(void *pl_ptr)
{
    Payload payload;

    //memset(pl_ptr, 0, MSG_MAX_PL_LENGTH);

    if (pipe == nullptr ||
        !xQueueReceive(pipe->request, &payload, (TickType_t)WAIT_TICKS))
    {
        return 0;
    }

    last_id = payload.id;
    memcpy(pl_ptr, (void*)&(payload.payload), payload.pl_size);
    return payload.pl_size;
}

/**
 * @brief
 *  Register task handler of server with pipe object
 *
 * @param[in] server_hdl - Server task handle
 */
void Msg::Server::registerServer(TaskHandle_t server_hdl)
{
    pipe->server = server_hdl;
}