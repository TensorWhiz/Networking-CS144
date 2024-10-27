#pragma once

#include "byte_stream.hh"
#include <vector>
//#include <set>
class Reassembler
{
public:
  // Construct Reassembler to write into given ByteStream.
  explicit Reassembler( ByteStream&& output ) : output_( std::move( output ) ),first_unassembled_index_(0),unacceptable_index_(0),bytes_pending_num_(0),last_byte_index_(-1),unordered_bytes_(output_.capacity_,'\0'),unordered_bytes_flag_(output_.capacity_,false) {}

  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring );

  // How many bytes are stored in the Reassembler itself?
  uint64_t bytes_pending() const;

  // Access output stream reader
  Reader& reader() { return output_.reader(); }
  const Reader& reader() const { return output_.reader(); }

  // Access output stream writer, but const-only (can't write from outside)
  const Writer& writer() const { return output_.writer(); }

private:
  void push_bytes( uint64_t first_index, std::string data ,bool is_last_substring);
  void cache_bytes( uint64_t first_index, std::string data);
  void flush_buffer(); 
  ByteStream output_; // the Reassembler writes to this ByteStream
  uint64_t first_unassembled_index_;
  uint64_t unacceptable_index_;
  uint64_t bytes_pending_num_;
  int last_byte_index_;
  std::string unordered_bytes_ ;
  std::vector<bool> unordered_bytes_flag_;

};

// class IntervalSet {
// public:
//     using Interval = std::pair<int, int>;

//     // 插入区间
//     void insert(const Interval& newInterval) {
//         auto it = intervals.lower_bound(newInterval);
        
//         // 查找所有与 newInterval 有可能重叠的区间
//         int start = newInterval.first, end = newInterval.second;
        
//         if (it != intervals.begin() && std::prev(it)->second >= start) {
//             --it; // 找到第一个可能重叠的区间
//         }

//         // 合并重叠区间
//         while (it != intervals.end() && it->first <= end) {
//             start = std::min(start, it->first);
//             end = std::max(end, it->second);
//             it = intervals.erase(it); // 删除旧的区间
//         }
        
//         // 插入合并后的区间
//         intervals.emplace(start, end);
//     }

//     // 判断区间是否重叠
//     bool isOverlapping(const Interval& newInterval) const {
//         auto it = intervals.lower_bound(newInterval);
        
//         if (it != intervals.end() && it->first <= newInterval.second) {
//             return true;
//         }
        
//         if (it != intervals.begin() && std::prev(it)->second >= newInterval.first) {
//             return true;
//         }
        
//         return false;
//     }

// private:
//     struct Compare {
//         bool operator()(const Interval& a, const Interval& b) const {
//             return a.first < b.first;
//         }
//     };

//     std::set<Interval, Compare> intervals;
// };

