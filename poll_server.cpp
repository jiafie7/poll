#include "socket/server_socket.h"
#include "socket/poller.h"

using namespace melon::socket;

int main()
{
  Singleton<LogSystem>::getInstance()->open("./../poll_server.log");

  ServerSocket server("127.0.0.1", 7777);

  Poller poller;
  poller.create(1024);

  poller.addFd(server.fd());

  while (true)
  {
    int ret = poller.poll(2000);
    if (ret < 0)
    {
      log_error("socket poll error: errno = %d, errmsg = %s.", errno, strerror(errno));
      break;
    }
    else if (ret == 0)
    {
      log_debug("socket poll timeout.");
      continue;
    }
    log_debug("socket poll ok: ret = %d.", ret);

    for (int i = 0; i < poller.maxFd() + 1; ++ i)
    {
      if (!(poller.isSet(i, POLLIN | POLLHUP | POLLERR)))
        continue;
      
      if (ret -- == 0)
        break;

      if (i == 0)
      {
        int conn_fd = server.accept();
        if (conn_fd < 0)
        {
          log_error("socket accept error: errno = %d, errmsg = %s.", errno, strerror(errno));
          continue;
        }
        Socket socket(conn_fd);
        socket.setNonBlocking();
        poller.addFd(conn_fd);
      }
      else
      {
        int conn_fd = poller.getFd(i);
        Socket client(conn_fd);

        char buf[1024] = {0};
        int len = client.recv(buf, sizeof(buf));

        if (len < 0)
        {
          if (errno == EAGAIN || errno == EWOULDBLOCK)
          {
            log_debug("socket recv would block: conn = %d, errno = %d, errmsg = %s.", conn_fd, errno, strerror(errno));
          }
          else if (errno == EINTR)
          {
            log_debug("socket recv interrupted: conn = %d, errno = %d, errmsg = %s.", conn_fd, errno, strerror(errno));
          }
          else
          {
            log_error("socket connection abort: conn = %d, errno = %d, errmsg = %s.", conn_fd, errno, strerror(errno));
            poller.delFd(conn_fd);
            client.close();
          }
        }
        else if (len == 0)
        {
          log_debug("socket closed by peer: conn = %d.", conn_fd); 
          poller.delFd(conn_fd);
          client.close();
        }
        else
        {
          log_debug("recv: conn = %d, msg = %s", conn_fd, buf);

          std::string msg = "hi, I am server!";
          client.send(msg.c_str(), msg.size());
        }
      }
    }
  }

  server.close();

  return 0;
}
