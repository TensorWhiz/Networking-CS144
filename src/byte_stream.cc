#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity )
  : capacity_( capacity ), bytes_pushed_( 0 ), bytes_popped_( 0 ), close_flag_( false )
{}

bool Writer::is_closed() const
{
  // Your code here.
  return close_flag_;
}

void Writer::push( string data )
{
  // Your code here.
  if ( is_closed() || has_error() )
    return;
  const size_t len = min( data.length(), available_capacity() );
  buf_.append( data.substr( 0, len ) );
  bytes_pushed_ += len;
  return;
}

void Writer::close()
{
  // Your code here.
  close_flag_ = true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return capacity_ - ( bytes_pushed_ - bytes_popped_ );
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return bytes_pushed_;
}

bool Reader::is_finished() const
{
  // Your code here.
  return close_flag_ && bytes_buffered() == 0;
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return bytes_popped_;
}

string_view Reader::peek() const
{
  // Your code here.
  return this->buf_;
}

void Reader::pop( uint64_t len )
{
  if ( bytes_buffered() == 0 ) {
    return;
  }
  len = min( len, buf_.length() );
  buf_.erase( 0, len );
  bytes_popped_ += len;
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return bytes_pushed_ - bytes_popped_;
}
