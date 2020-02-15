#ifndef EVENTTEST_H
#define EVENTTEST_H
#include <string>
#include "Event.h"
#include "EventId.h"

using namespace std;
class EventTest : public Event
{
public:
    EVENT_OBJECT(EventTest)
    EventTest();
    EventTest(string v1, string v2);
    ~EventTest();
    string value1() { return m_test_value1;}
    string value2() { return m_test_value2;}
    virtual int serialize_to_buffer(char *buf);
    virtual int serialize_from_buffer(char *buf);
    virtual int serialize_buffer_len();
    virtual int event_clear();
private:
    string m_test_value1;
    string m_test_value2;
};

#endif // EVENTTEST_H
