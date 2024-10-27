#include "reassembler.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  Writer& bytes_writer = output_.writer();
  unacceptable_index_ = first_unassembled_index_ + bytes_writer.available_capacity();
  if(is_last_substring)last_byte_index_=(int)(first_index+data.size()-1);

  if ( bytes_writer.is_closed() || ( bytes_writer.available_capacity() == 0 )
       || first_index > unacceptable_index_ || first_index+data.size()-1<first_unassembled_index_ ) {
  if(first_unassembled_index_==(uint64_t)(last_byte_index_+1) && last_byte_index_!=-1){
    output_.writer().close(); // 关闭写端
    unordered_bytes_.clear(); // 清空缓冲区
    bytes_pending_num_ = 0;
  }
    return;
  }

  if ( first_index + data.size() > unacceptable_index_ ) {
    data.resize( unacceptable_index_ - first_index );
  }

  uint64_t size = bytes_writer.available_capacity() - unordered_bytes_.size();
  unordered_bytes_.resize( bytes_writer.available_capacity(), '\0' );
  for ( uint64_t i = 0; i < size;i++) {
    unordered_bytes_flag_.push_back( false );
  }

  

  if ( first_index > first_unassembled_index_ ) {
    cache_bytes( first_index, data );
  } else {
    push_bytes( first_index,data ,is_last_substring );
  }
  flush_buffer();
  
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return bytes_pending_num_;
}

void Reassembler::push_bytes( uint64_t first_index, string data ,bool is_last_substring)
{
  if ( first_index < first_unassembled_index_ )
    data.erase( 0, first_unassembled_index_ - first_index );
  first_unassembled_index_ += data.size();
  for ( uint64_t i = 0; i < data.size(); i++ ) {
    unordered_bytes_.erase( 0, 1 );
    if(unordered_bytes_flag_.front())bytes_pending_num_--;
    unordered_bytes_flag_.erase( unordered_bytes_flag_.begin() );
    
  }
  output_.writer().push(data );
  if ( is_last_substring ) {
    output_.writer().close(); // 关闭写
    unordered_bytes_.clear(); // 清空缓冲区
    bytes_pending_num_ = 0;
  }
}

void Reassembler::cache_bytes( uint64_t first_index, string data)
{
  uint64_t pos = first_index - first_unassembled_index_;
  for ( char c : data ) {
    if ( !unordered_bytes_flag_.at( pos ) ) {
      unordered_bytes_flag_.at( pos ) = true;
      unordered_bytes_.at( pos ) = c;
      bytes_pending_num_++;
    }
    pos++;
  }
}

void Reassembler::flush_buffer()
{
  std::string s = "";
  while ( !unordered_bytes_.empty() &&unordered_bytes_flag_.front() ) {
    //bool b = unordered_bytes_flag_.front();
    //if ( b ) {
      // Write the byte to the output stream
      s.push_back( unordered_bytes_.front() );
      unordered_bytes_.erase( 0,1 );
      unordered_bytes_flag_.erase( unordered_bytes_flag_.begin() );
      bytes_pending_num_--;       // Decrement pending bytes count
      first_unassembled_index_++; // Move to the next index
    // } else {
    //   break; // If we hit a missing byte, stop flushing
    // }
  }
  if ( !s.empty() )
    output_.writer().push( s );
  if(first_unassembled_index_==(uint64_t)(last_byte_index_+1) && last_byte_index_!=-1){
    output_.writer().close(); // 关闭写端
    unordered_bytes_.clear(); // 清空缓冲区
    bytes_pending_num_ = 0;
  }

}
