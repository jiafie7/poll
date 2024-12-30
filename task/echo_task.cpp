#include "task/echo_task.h"

using namespace melon::task;

EchoTask::EchoTask(int socket_fd)
  : m_socket_fd(socket_fd)
{
}
      
EchoTask::~EchoTask()
{
  destroy();
}
      
bool EchoTask::run()
{
  log_debug("echo task run.");

  Socket client(m_socket_fd);

  char buf[1024] = {0};
  int len = client.recv(buf, sizeof(buf));
  
  if (len < 0)
  {
    if (errno == EAGAIN || errno == EWOULDBLOCK)
    {
      log_debug("socket recv would block: conn = %d, errno = %d, errmsg = %s.", m_socket_fd, errno, strerror(errno));
      return true;
    }
    else if (errno == EINTR)
    {
      log_debug("socket recv interrupted: conn = %d, errno = %d, errmsg = %s.", m_socket_fd, errno, strerror(errno));
      return true;
    }
    else
    {
      log_error("socket connection abort: conn = %d, errno = %d, errmsg = %s.", m_socket_fd, errno, strerror(errno));
      return false;
    }
  }
  else if (len == 0)
  {
    log_debug("socket closed by peer: conn = %d, errno = %d, errmsg = %s.", m_socket_fd, errno, strerror(errno));
    return false;
  }
  else
  {
    log_debug("recv: conn = %d, msg = %s", m_socket_fd, buf);

    std::string msg = "hi, I am server!";
    client.send(msg.c_str(), msg.size());
    return true;
  }
}
      
void EchoTask::destroy()
{
  log_debug("echo task destroy.");
}
