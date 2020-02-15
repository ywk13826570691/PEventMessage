#include "client/ClientObserver.h"
#include "event/EventTest.h"
#include <string>
using namespace std;
class TestClient : public ClientObserver
{
public:
    TestClient(string name);
    ~TestClient();
    virtual void handle_event(EventTest *e);
};

TestClient::TestClient(string name):
    ClientObserver(name)
{
	subscribe_event(0,EVENT_ID_NULL);
}

TestClient::~TestClient()
{

}

void TestClient::handle_event(EventTest *e)
{
    printf("%s handle event test:%s %s\n", client_name().c_str(), e->value1().c_str(), e->value2().c_str());
}

int main(int argc, char**argv)
{
    TestClient testCl2("testClient2");
    while(1)
    {
        testCl2.exec();
    }
}
