#pragma once

#include "utils/SpinLock.h"

#if !PL_EVENTFD_UNSUPPORTED
#if !PL_WINDOWS
#include <poll.h>
#include <sched.h>
#endif

#include <utility>
#include <vector>

namespace pl
{
	template <class ValueT>
	class MpscPollableQueue
	{
		 public:
			int reader_wait_nonblock()
			{
				auto ready = reader_vector_.size() - reader_pos_;
				if (ready != 0)
				{

					return (int)ready;
				}

				auto guard = lock_.lock();
				if (writer_vector_.empty())
				{

					wait_event_fd_ = true;
					return 0;
				}
				else
				{
					reader_vector_.clear();
					reader_pos_ = 0;
					std::swap(writer_vector_, reader_vector_);

					return (int)reader_vector_.size();
				}
			}
			ValueT reader_get_unsafe()
			{
				return std::move(reader_vector_[reader_pos_++]);
			}
			void reader_flush()
			{
				
			}
			void writer_put(ValueT value)
			{
				auto guard = lock_.lock();
				writer_vector_.push_back(std::move(value));
				if (wait_event_fd_)
				{
					wait_event_fd_ = false;

				}
			}




			void writer_flush()
			{
				
			}

			void init()
			{

			}
			void destroy()
			{

				{


					writer_vector_.clear();
					reader_vector_.clear();
					reader_pos_ = 0;
				}
			}


#if !PL_WINDOWS && 0
			int reader_wait()
			{
				int res;

				while ((res = reader_wait_nonblock()) == 0)
				{
					
					pollfd fd;
					fd.fd = reader_get_event_fd().get_fd().get_native_fd();
					fd.events = POLLIN;
					poll(&fd, 1, -1);
				}
				return res;
			}
#endif

		private:
			SpinLock lock_;
			bool wait_event_fd_{false};

			std::vector<ValueT> writer_vector_;
			std::vector<ValueT> reader_vector_;
			size_t reader_pos_{0};
	};

}  

#else
#include "td/utils/logging.h"

namespace pl {



template <class T>
class MpscPollableQueue {
 public:
  using ValueType = T;

  void init() {
    UNREACHABLE();
  }

  template <class PutValueType>
  void writer_put(PutValueType &&value) {
    UNREACHABLE();
  }

  void writer_flush() {
    UNREACHABLE();
  }

  int reader_wait_nonblock() {
    UNREACHABLE();
    return 0;
  }

  ValueType reader_get_unsafe() {
    UNREACHABLE();
    return ValueType();
  }

  void reader_flush() {
    UNREACHABLE();
  }

  MpscPollableQueue() = default;
  MpscPollableQueue(const MpscPollableQueue &) = delete;
  MpscPollableQueue &operator=(const MpscPollableQueue &) = delete;
  MpscPollableQueue(MpscPollableQueue &&) = delete;
  MpscPollableQueue &operator=(MpscPollableQueue &&) = delete;
  ~MpscPollableQueue() = default;
};

}  

#endif
