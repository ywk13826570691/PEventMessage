#include "Event.h"
#include "EventId.h"
#include "EventTest.h"
Event *gAllEvents[EVENT_ID_MAX] =
{
    new EventTest(),//EVENT_ID_TEST
    NULL,//EVENT_ID_10MS_TIMER
};
