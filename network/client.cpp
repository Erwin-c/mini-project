export module idc.network:client;

import <iostream>;

import <algorithm>;
import <string>;

import <cstring>;

import <netdb.h>;
import <poll.h>;
import <signal.h>;
import <sys/socket.h>;

namespace idc::network {
/**
 *  @brief  Socket 通讯的客户端类.
 */
export class TcpClient {
 public:
  /**
   *  @brief  构造函数.
   */
  TcpClient() {
    _connfd = -1;
    _ip.clear();
    _port = 0;
    _btimeout = false;
    _buflen = 0;
  }

  /**
   *  @brief  析构函数, 自动关闭 Socket, 释放资源.
   */
  ~TcpClient() { closeConnection(); }

  /**
   *  @brief  向服务端发起连接请求.
   *  @param  ip  服务端的 IP 地址.
   *  @param  port  服务端监听的端口.
   *  @return  True 连接成功.
   */
  bool connectServer(const std::string &ip, const unsigned int port) {
    // 如果已连接到服务端, 则断开, 这种处理方法没有特别的原因, 不要纠结.
    if (_connfd != -1) {
      close(_connfd);
      _connfd = -1;
    }

    // 忽略 SIGPIPE 信号, 防止程序异常退出.
    // 如果 send() 到一个 disconnected Socket 上, 内核就会发出 SIGPIPE 信号.
    // 这个信号的缺省处理方法是终止进程, 大多数时候这都不是我们期望的.
    // 我们重新定义这个信号的处理方法, 大多数情况是直接屏蔽它.
    signal(SIGPIPE, SIG_IGN);

    if ((_connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      return false;
    }

    hostent *h = nullptr;
    if ((h = gethostbyname(ip.c_str())) == nullptr) {
      close(_connfd);
      _connfd = -1;
      return false;
    }

    sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    // 指定服务端的通讯端口.
    servaddr.sin_port = htons(port);
    memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);

    if (connect(_connfd, (sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
      close(_connfd);
      _connfd = -1;
      return false;
    }

    _ip = ip;
    _port = port;

    std::cout << _ip << std::endl;
    std::cout << _port << std::endl;

    return true;
  }

  /**
   *  @brief  接收服务端发送过来的数据.
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
   *  @brief  向服务端发送数据.
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

    std::cout << "buffer: " << buffer << std::endl;
    std::cout << "buffer: " << sizeof(*buffer) << std::endl;

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
   *  @brief  断开与服务端的连接.
   */
  void closeConnection() {
    if (_connfd > 0) {
      close(_connfd);
    }

    _connfd = -1;
    _ip.clear();
    _port = 0;
    _btimeout = false;
    _buflen = 0;

    return;
  }

 private:
  int _connfd;      // 客户端的 Socket.
  std::string _ip;  // 服务端的 IP 地址.
  int _port;        // 与服务端通讯的端口.
  bool _btimeout;   // tcpRead() 失败的原因是否是超时.
  int _buflen;      // tcpRead() 接收的报文大小, 单位: 字节.
};
}  // namespace idc::network
