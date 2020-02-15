#include "EventTest.h"
#include "Common.h"
#include <string.h>
#include <stdlib.h>


typedef struct EventTestBuffer
{
    char test_value1[50];
    char test_value2[50];
}__ALIGN__ EventTestBuffer;
/*
or we using this serialize
typedef struct EventTestBuffer
{
    int test_value1_len;
    int test_value2_len;
}__ALIGN__ EventTestBuffer;

*/

EventTest::EventTest():
    Event(EVENT_ID_TEST, "EventTest")
{
    m_test_value1 = "";
    m_test_value2 = "";
}

EventTest::EventTest(string v1, string v2):
    Event(EVENT_ID_TEST, "EventTest")
{
    m_test_value1 = v1;
    m_test_value2 = v2;
}

EventTest::~EventTest()
{

}

int EventTest::serialize_to_buffer(char *buf)
{
    int len = Event::serialize_to_buffer(buf);

    EventTestBuffer test_base;
    strcpy(test_base.test_value1, m_test_value1.c_str());
    strcpy(test_base.test_value2, m_test_value2.c_str());
    memcpy(buf+len, &test_base, sizeof(test_base));
    len += sizeof(EventTestBuffer);
    return len;
}

int EventTest::serialize_from_buffer(char *buf)
{
    int len = Event::serialize_from_buffer(buf);
    EventTestBuffer *test_base = (EventTestBuffer *)(buf+len);
    m_test_value1 = string(test_base->test_value1);
    m_test_value2 = string(test_base->test_value2);
    len += sizeof(EventTestBuffer);
    return 0;
}

int EventTest::serialize_buffer_len()
{
    int len = 0;
    len = Event::serialize_buffer_len();
    len += sizeof(EventTestBuffer);
    return len;
}

int EventTest::event_clear()
{
    Event::event_clear();
    m_test_value1="";
    m_test_value2="";
}

