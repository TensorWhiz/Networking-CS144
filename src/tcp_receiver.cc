#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  // Your code here.
  // 成功接收并确认的数据字节数
  const uint64_t checkpoint = reassembler_.writer().bytes_pushed() + ISN_.has_value();
  if ( message.RST ) {
    reassembler_.reader().set_error();
  }
  // 一个重复的初始SYN消息,忽略
  else if ( checkpoint > 0 && checkpoint <= UINT32_MAX && message.seqno == ISN_ )
    return;

  if ( !ISN_.has_value() ) {
    if ( !message.SYN )
      return;//如果没有SYN，则无法建立连接，直接忽略
    ISN_ = message.seqno;
  }
  const uint64_t abs_seqno_ = message.seqno.unwrap( *ISN_, checkpoint );
  reassembler_.insert( abs_seqno_ == 0 ? abs_seqno_ : abs_seqno_ - 1, move( message.payload ), message.FIN );//调用数据重组器的insert方法，将当前消息的有效负载和结束标志（FIN）插入到接收缓冲区。
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  const uint64_t checkpoint = reassembler_.writer().bytes_pushed() + ISN_.has_value();
  const uint64_t capacity = reassembler_.writer().available_capacity();
  const uint16_t wnd_size = capacity > UINT16_MAX ? UINT16_MAX : capacity;// 发送给对端的窗口大小
  if ( !ISN_.has_value() )//如果初始序列号尚未设置，错误
    return { {}, wnd_size, reassembler_.writer().has_error() };
  //确认序列号通过Wrap32::wrap方法，将64位的绝对序列号映射到32位空间。如果连接已关闭（is_closed为真），确认序列号需要额外增加1，表示确认FIN标志。
  return { Wrap32::wrap( checkpoint + reassembler_.writer().is_closed(), *ISN_ ),
           wnd_size,
           reassembler_.writer().has_error() };
}