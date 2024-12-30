#pragma once

#include "socket/socket.h"

using namespace melon::socket;

namespace melon
{
  namespace task
  {
    class EchoTask
    {
    public:
      EchoTask() = delete;
      EchoTask(int socket_fd); 
      ~EchoTask();

      bool run();
      void destroy();

    private:
      int m_socket_fd = 0;
    };
  }
}
