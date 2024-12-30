#include "socket/socket_handler.h"

using namespace melon::socket;

int main()
{
  Singleton<LogSystem>::getInstance()->open("./../handler.log");

  SocketHandler* handler = Singleton<SocketHandler>::getInstance();

  handler->listen("127.0.0.1", 7777);

  handler->handle(1024, -1);

  return 0;
}
