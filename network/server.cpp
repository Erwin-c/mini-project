export module idc.network:server;

import <cstring>;

import <arpa/inet.h>;
import <netinet/in.h>;
import <poll.h>;
import <signal.h>;
import <sys/socket.h>;

namespace idc::network {
/**
 *  @brief  Socket 通讯的服务端类.
 */
export class TcpServer {
 public:
  /**
   *  @brief  构造函数.
   */
  TcpServer() {
    _socklen = 0;
    memset(&_clientaddr, 0, sizeof(_clientaddr));
    memset(&_servaddr, 0, sizeof(_servaddr));
    _listenfd = -1;
    _connfd = -1;
    _btimeout = false;
    _buflen = 0;
  };

  /**
   *  @brief  析构函数, 自动关闭 Socket, 释放资源.
   */
  ~TcpServer() {
    closeListen();
    closeClient();
  };

  /**
   *  @brief  服务端初始化.
   *  @param  port  服务端监听的端口.
   *  @param  backlog  未完成连接队列的最大长度.
   *  @return  True 初始化成功.
   *
   *  一般情况下, 只要 port 设置正确, 没有被占用, 初始化都会成功.
   */
  bool initServer(const unsigned int port, const int backlog = 5) {
    // 如果服务端的 Socket > 0, 关掉它, 这种处理方法没有特别的原因, 不要纠结.
    if (_listenfd > 0) {
      close(_listenfd);
      _listenfd = -1;
    }

    if ((_listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      return false;
    }

    // 忽略 SIGPIPE 信号, 防止程序异常退出.
    signal(SIGPIPE, SIG_IGN);

    // 打开 SO_REUSEADDR 选项.
    // 当服务端连接处于 TIME_WAIT 状态时, 可以再次启动服务器.
    // 否则 bind() 可能会不成功, 报： Address already in use.
    // char opt = 1; unsigned int len = sizeof(opt);
    int opt = 1;
    unsigned int len = sizeof(opt);
    setsockopt(_listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, len);

    memset(&_servaddr, 0, sizeof(_servaddr));
    _servaddr.sin_family = AF_INET;
    _servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // 任意 IP 地址.
    _servaddr.sin_port = htons(port);
    if (bind(_listenfd, (sockaddr *)&_servaddr, sizeof(_servaddr)) != 0) {
      closeListen();
      return false;
    }

    if (listen(_listenfd, backlog) != 0) {
      closeListen();
      return false;
    }

    return true;
  }

  /**
   *  @brief  阻塞等待客户端的连接请求.
   *  @return  True 有新的客户端已连接上来成功.
   *
   *  如果 tcpAccept() 失败被中断，可以重新 tcpAccept().
   */
  bool tcpAccept() {
    if (_listenfd == -1) {
      return false;
    }

    _socklen = sizeof(sockaddr_in);

    if ((_connfd = accept(_listenfd, (sockaddr *)&_clientaddr,
                          (socklen_t *)&_socklen)) < 0) {
      return false;
    }

    return true;
  }

  /**
   *  @brief  获取客户端的 IP 地址.
   *  @return  客户端的 IP 地址, 如 "192.168.1.100".
   */
  char *getIP() { return inet_ntoa(_clientaddr.sin_addr); }

  /**
   *  @brief  接收客户端发送过来的数据.
   *  @param  buffer  接收数据缓冲区的地址, 数据的长度存放在 _buflen 成员变量中.
   *  @param  itimeout  等待数据的超时时间, 单位: 秒, 缺省值是 0-无限等待.
   *  @return  True 接收成功.
   *
   *  失败有两种情况:
   *    1) 等待超时, 成员变量 _btimeout 的值被设置为 true.
   *    2) Socket 连接已不可用.
   */
  bool tcpRead(char *buffer, const int itimeout = 0) {
    if (_connfd == -1) {
      return false;
    }

    // 如果 itimeout > 0, 表示需要等待 itimeout 秒,
    // 如果 itimeout 秒后还没有数据到达, 返回 false.
    if (itimeout > 0) {
      pollfd fds;
      fds.fd = _connfd;
      fds.events = POLLIN;
      int iret;
      _btimeout = false;
      if ((iret = poll(&fds, 1, itimeout * 1000)) <= 0) {
        if (iret == 0) {
          _btimeout = true;
        }

        return false;
      }
    }

    _buflen = 0;
    if (recv(_connfd, (void *)&_buflen, 4, 0) <= 0) {
      return false;
    }

    // 把报文长度由网络字节序转换为主机字节序.
    _buflen = ntohl(_buflen);

    int nLeft = _buflen;  // 剩余需要读取的字节数.
    int idx = 0;          // 已成功读取的字节数.
    int nread = 0;        // 每次调用 recv() 读到的字节数.
    while (nLeft > 0) {
      if ((nread = recv(_connfd, buffer + idx, nLeft, 0)) <= 0) {
        return false;
      }

      idx = idx + nread;
      nLeft = nLeft - nread;
    }

    return true;
  }

  /**
   *  @brief  向客户端发送数据.
   *  @param  buffer  待发送数据缓冲区的地址.
   *  @param  ibuflen: 待发送数据的大小, 单位: 字节, 缺省值为0.
   *  @return  True 发送成功.
   *
   *  ibuflen:
   *    1) 如果发送的是 ASCII 字符串, ibuflen 取 0.
   *    2) 如果是二进制流数据, ibuflen 为二进制数据块的大小.
   *
   *  如果失败, 表示 Socket 连接已不可用.
   */
  bool tcpWrite(const char *buffer, const int ibuflen = 0) {
    if (_connfd == -1) {
      return false;
    }

    int ilen = 0;
    // 如果 ibuflen == 0, 就认为需要发送的是字符串, 报文长度为字符串的长度.
    if (ibuflen == 0) {
      ilen = strlen(buffer);
    } else {
      ilen = ibuflen;
    }

    // 把报文长度转换为网络字节序.
    int ilenn = htonl(ilen);

    char TBuffer[ilen + 4] = {0};       // 发送缓冲区.
    memcpy(TBuffer, &ilenn, 4);         // 把报文长度拷贝到缓冲区.
    memcpy(TBuffer + 4, buffer, ilen);  // 把报文内容拷贝到缓冲区.

    int nLeft = ilen;  // 剩余需要写入的字节数.
    int idx = 0;       // 已成功写入的字节数.
    int nwritten = 0;  // 每次调用 send() 函数写入的字节数.
    while (nLeft > 0) {
      if ((nwritten = send(_connfd, TBuffer + idx, nLeft, 0)) <= 0) {
        return false;
      }

      nLeft = nLeft - nwritten;
      idx = idx + nwritten;
    }

    return true;
  }

  /**
   *  @brief  闭监听的 Socket, 即 _listenfd.
   *
   *  常用于多进程服务程序的子进程代码中.
   */
  void closeListen() {
    if (_listenfd > 0) {
      close(_listenfd);
      _listenfd = -1;
    }

    return;
  }

  /**
   *  @brief  关闭客户端的 Socket, 即 _connfd.
   *
   *  常用于多进程服务程序的父进程代码中.
   */
  void closeClient() {
    if (_connfd > 0) {
      close(_connfd);
      _connfd = -1;
    }

    return;
  }

 private:
  int _socklen;             // 结构体 sockaddr_in 的大小.
  sockaddr_in _clientaddr;  // 客户端的地址信息.
  sockaddr_in _servaddr;    // 服务端的地址信息.
  int _listenfd;            // 服务端用于监听的 Socket.
  int _connfd;              // 客户端连接上来的 Socket.
  bool _btimeout;           // tcpRead() 失败的原因是否是超时.
  int _buflen;              // tcpRead() 接收的报文大小, 单位: 字节.
};
}  // namespace idc::network
