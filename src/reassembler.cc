#include "reassembler.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  if (output_.writer().is_closed()) return;

  if (is_last_substring) {
    last_byte_index_ = first_index + data.size();
  }

  uint64_t unacceptable_index = first_unassembled_index_ + output_.writer().available_capacity();
  if (first_index >= unacceptable_index) return;
  
  if (first_index < first_unassembled_index_) {
    data.erase(0, first_unassembled_index_ - first_index);
    first_index = first_unassembled_index_;
}

  if (first_index + data.size() > unacceptable_index) {
    data.resize(unacceptable_index - first_index);
  }

  if(first_index+data.size()<first_unassembled_index_)return;
  else{
  Interval new_interval{first_index, first_index + data.size(), data};

  write_and_merge(new_interval);
}

  if (first_unassembled_index_ == static_cast<uint64_t>(last_byte_index_)) {
    output_.writer().close();
  }
}


uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return bytes_pending_num_;
}

void Reassembler::write_and_merge(Interval new_interval) {
  auto it = intervals_.lower_bound(new_interval);
  if (it != intervals_.begin() && std::prev(it)->end>= new_interval.start) {
      --it;
  }
  Interval tmp{new_interval.start,new_interval.end,new_interval.data};

  while (it != intervals_.end() && it->start < tmp.end) {
      if(it->start>=tmp.start && it->end>=tmp.end){
        tmp.data +=it->data.substr(tmp.end-it->start,it->end-tmp.end);
        tmp.end=it->end;
      }
      else if (it->start<=tmp.start && it->end<=tmp.end){
        tmp.data = it->data+tmp.data.substr(it->end-tmp.start,tmp.end-it->end);
        tmp.start=it->start;
      }
      else if(it->start<=tmp.start && it->end>=tmp.end){
        tmp.data=it->data;
        tmp.start=it->start;
        tmp.end=it->end;
        
      }
      bytes_pending_num_ -= it->data.size(); 
      it = intervals_.erase(it);
  }

  intervals_.insert(tmp);
  bytes_pending_num_ += tmp.data.size();

  while (!intervals_.empty() && intervals_.begin()->start == first_unassembled_index_) {
        const auto& interval = *intervals_.begin();
        output_.writer().push(interval.data);
        bytes_pending_num_ -= interval.data.size();
        first_unassembled_index_ = interval.end;
        intervals_.erase(intervals_.begin());
  }
}