/**
 * @file msg.h
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
#ifndef _TRANSPORT_H_
#define _TRANSPORT_H_

#include <stdint.h>
#include <stdbool.h>
#include <msg_config.h>
#include <FreeRTOS_SAMD21.h>

/**
 * @brief
 *  Namespace for interface
 *
 */
namespace Msg
{
    /**
     * @brief
     *  Struct implementing a software pipe using two FreeRTOS provided queues
     *
     */
    struct Pipe
    {
        /** Server task, used to resume */
        TaskHandle_t server;
        /** Request queue (client request from server) */
        QueueHandle_t request;
        /** Response queue (server responds to client) */
        QueueHandle_t response;

        /**
         * @brief
         *  Construct an empty Pipe object
         *
         */
        Pipe(void);

        /**
         * @brief
         *  Construct a new Pipe object
         *
         * @param[in] buffer_size - Elements in each queue
         */
        Pipe(uint16_t buffer_size);
    };

    /**
     * @brief
     *  Client class, used to request from server
     *
     */
    class Client
    {
    private:
        /** Queue shared between server and clients */
        Pipe *pipe;
        /** Client id */
        uint8_t id;

    public:
        /**
         * @brief
         *  Construct a new Client object
         *
         */
        Client(void);

        /**
         * @brief
         *  Construct a new Client object
         *
         * @param[in] pipe - Queues shared by server/client
         */
        Client(Pipe *pipe);

        /**
         * @brief
         *  Set client ID
         *
         * @param[in] id - ID byte
         */
        void idSet(uint8_t id);

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
        uint8_t send(void *pl_ptr, uint8_t size);

        /**
         * @brief
         *  Receive response from server
         *
         * @param[out] pl_ptr - Pointer to data to send
         *
         * @return
         *  Bytes received
         */
        uint8_t receive(void *pl_ptr);
    };

    /**
     * @brief
     *  Server class, responds to requests from clients
     *
     */
    class Server
    {
    private:
        /** Queue shared between server and clients */
        Pipe *pipe;
        /** ID of the last received message */
        uint8_t last_id;

    public:
        /**
         * @brief Construct a new Server object
         *
         */
        Server(void);

        /**
         * @brief Construct a new Server object
         *
         * @param[in] pipe - Queues shared by server/client
         */
        Server(Pipe *pipe);

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
        uint8_t send(void *pl_ptr, uint8_t size);

        /**
         * @brief
         *  Receive a request from a client
         *
         * @param[out] pl_ptr - Pointer to received data
         *
         * @return
         *  Number of bytes received
         */
        uint8_t receive(void *pl_ptr);

        /**
         * @brief
         *  Register task handler of server with pipe object
         *
         * @param[in] server_hdl - Server task handle
         */
        void registerServer(TaskHandle_t server_hdl);
    };
}

#endif /* _TRANSPORT_H_ */