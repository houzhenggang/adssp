#ifndef  __PID_TCP_H
#define  __PID_TCP_H 



struct tcp_hdr_s {
    uint16_t src;       //16位源端口号
    uint16_t dest;         //16位目的端口号

                          //每个tcp段都包源和目的端口号，用于寻找发送端和接受端的应用进程。这两个端口号加上ip报头中的源ip和目的ip，来确定一个唯一的TCP连接。
    uint32_t seq;          //此次发送的数据在整个报文段中的起始字节数。此序号用来标识从tcp发送端向tcp接受端发送的数据字节流，seq表示在这个报文段中的第一个数据字节。如果将字节流看做在两个应用程序间的单向流动，则tcp用序号对每个字节进行计数。32 bit的无符号数。为了安全起见，它的初始值是一个随机生成的数，它到达2的32次方-1后又从零开始。
    uint32_t ack_seq;      //是下一个期望接收的字节，确认序号应当是上次已成功接收的序号+1，只有ack标志为1时确认序号字段才有效。一旦一个连接已经建立了，ack总是=1
#if defined(__LITTLE_ENDIAN_BITFIELD)  //小端
    uint16_t res1:4,  		// 保留位
    doff:4,  //tcp头部长度，指明了在tcp头部中包含了多少个32位的字。由于options域的长度是可变的，所以整个tcp头部的长度也是变化的。4bit可表示最大值15，故15*32=480bit=60字节，所以tcp首部最长60字节。然后，没有任选字段，正常的长度是20字节
    fin:1,  //发端完成发送任务
    syn:1,  //同步序号用来发起一个连接
    rst:1,  //重建连接
    psh:1,  //接收方应该尽快将这个报文段交给应用层
    ack:1,  //一旦一个连接已经建立了，ack总是=1

    urg:1,  //紧急指针有效
    ece:1, 
    cwr:1;
#elif defined(__BIG_ENDIAN_BITFIELD)
     uint16_t doff:4,
      res1:4,
      cwr:1,
      ece:1,
      urg:1,
      ack:1,
      psh:1,
      rst:1,
      syn:1,
      fin:1;
#else
#error "Adjust your <asm/byteorder.h> defines"
#endif 
 uint16_t window;   //窗口大小，单位字节数，指接收端正期望接受的字节，16bit，故窗口大小最大为16bit=1111 1111 1111 1111（二进制）=65535（十进制）字节
 uint16_t check;    //校验和校验的是整个tcp报文段，包括tcp首部和tcp数据，这是一个强制性的字段，一定是由发端计算和存储，并由收端进行验证。
 uint16_t urg_ptr;
};


#define TCP_HDR_LEN(hdr) ((hdr->doff)*4)

#define TCP_HLEN 20

berr pid_tcp(struct pbuf *p, hytag_t *hytag, int inner_outer);

#endif

