#include "EventHandle.h"
#include "Event.h"

extern Event *gAllEvents[];

EventHandle::EventHandle()
{

}

EventHandle::~EventHandle()
{

}

int EventHandle::event_handler(int event_id, char *event_data)
{
    if(event_id > EVENT_ID_MAX || event_id < 0)
    {
        return -1;
    }

    Event *e = gAllEvents[event_id];
    if(e)
    {
        e->event_clear();
        e->serialize_from_buffer(event_data);
        e->event_handler(this);
    }
    return 0;
}

