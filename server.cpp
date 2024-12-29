#include <poll.h>

#include "socket/server_socket.h"

using namespace melon::socket;

#define MAX_CONN 1024

int main()
{
  Singleton<LogSystem>::getInstance()->open("./../server.log");

  ServerSocket server("127.0.0.1", 7777);

  struct pollfd fds[MAX_CONN];
  for (int i = 0; i < MAX_CONN; ++ i)
    fds[i].fd = -1;

  fds[0].fd = server.fd();
  fds[0].events = POLLIN;

  int max_fd = 0;

  while (true)
  {
    int ret = ::poll(fds, max_fd + 1, -1);
    if (ret < 0)
    {
      log_error("socket poll error: errno = %d, errormsg = %s.", errno, strerror(errno));
      break;
    }
    if (ret == 0)
    {
      log_debug("poll timeout.");
      continue;
    }
    log_debug("poll ok: ret = %d.", ret);

    for (int i = 0; i < max_fd + 1; ++ i)
    {
      if (!(fds[i].revents & POLLIN))
        continue;
      
      if (ret -- == 0)
        break;

      if (i == 0)
      {
        int conn_fd = server.accept();
        if (conn_fd < 0)
        {
          log_error("server accept error: errno = %d, errmsg = %s.", errno, strerror(errno));
          continue;
        }

        Socket socket(conn_fd);
        socket.setNonBlocking();

        for (int j = 1; j < MAX_CONN; ++ j)
        {
          if (fds[j].fd == -1)
          {
            fds[j].fd = conn_fd;
            fds[j].events = POLLIN;

            if (max_fd < j)
              max_fd = j;

            break;
          }
        }
      }
      else
      {
        int fd = fds[i].fd;
        Socket client(fd);
        
        char buf[1024] = {0};
        int len = client.recv(buf, sizeof(buf));

        if (len < 0)
        {
          if (errno == EAGAIN || errno == EWOULDBLOCK)
          {
            log_debug("socket recv would block: conn = %d, errno = %d, errmsg = %s.", fd, errno, strerror(errno));
          }
          else if (errno == EINTR)
          {
            log_debug("socket recv interrupted: conn = %d, errno = %d, errmsg = %s.", fd, errno, strerror(errno));
          }
          else
          {
            log_error("socket connection abort: conn = %d, errno = %d, errmsg = %s.", fd, errno, strerror(errno));
            fds[i].fd = -1;
            client.close();
          }
        }
        else if (len == 0)
        {
          log_debug("socket closed by peer: conn = %d.", fd);
          fds[i].fd = -1;
          client.close();
        }
        else
        {
          log_debug("recv: conn = %d, msg = %s", fd, buf);

          std::string msg = "hi, I am server!";
          client.send(msg.c_str(), msg.size());
        }
      }
    }
  }

  server.close();

  return 0;
}
