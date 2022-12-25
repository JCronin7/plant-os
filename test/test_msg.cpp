#include <msg.h>
#include <unity.h>

Msg::Pipe pipe(16);

Msg::Server server;
Msg::Client client0;
Msg::Client client1;

char se_buffer[32] = "test0";
char re_buffer[32];

void msg_init(void)
{
    server  = Msg::Server(&pipe);
    client0 = Msg::Client(&pipe);
    client1 = Msg::Client(&pipe);
    client0.idSet(0);
    client1.idSet(1);
}

void test_msg(void)
{
    client0.send(se_buffer, 5);
    uint8_t size = server.receive(re_buffer);
    uint32_t ret;

    TEST_ASSERT_EQUAL_STRING("test0", re_buffer);
    TEST_ASSERT_EQUAL(5, size);

    size = server.receive(re_buffer);
    TEST_ASSERT_EQUAL(0, size);

    client0.send(se_buffer, 5);
    strcpy(se_buffer, "test1");
    client0.send(se_buffer, 5);
    strcpy(se_buffer, "test2");
    client0.send(se_buffer, 5);
    strcpy(se_buffer, "test3");
    client0.send(se_buffer, 5);
    strcpy(se_buffer, "test4");
    client0.send(se_buffer, 5);
    strcpy(se_buffer, "new server");
    client1.send(se_buffer, 10);

    for (int i = 0; i < 6; i++)
    {
        size = server.receive(re_buffer);
        ret = i;
        server.send(&ret, 4);
    }

    size = client1.receive(&ret);
    TEST_ASSERT_EQUAL(0, size);

    client0.receive(&ret);
    TEST_ASSERT_EQUAL(0, ret);
    client0.receive(&ret);
    TEST_ASSERT_EQUAL(1, ret);
    client0.receive(&ret);
    TEST_ASSERT_EQUAL(2, ret);
    client0.receive(&ret);
    TEST_ASSERT_EQUAL(3, ret);
    client0.receive(&ret);
    TEST_ASSERT_EQUAL(4, ret);

    size = client1.receive(&ret);
    TEST_ASSERT_EQUAL(5, ret);
}
