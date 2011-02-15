#ifndef QNT_CORE_UTIL_QUEUE_HPP
#define QNT_CORE_UTIL_QUEUE_HPP

#include <deque>
#include <boost/thread.hpp>

template <typename T>
class queue {
public:
  void push(const T& t) {
    boost::mutex::scoped_lock lock(m_queue_mutex);
    bool was_empty = m_queue.empty();
    m_queue.push_back(t);
    lock.unlock();
    if(was_empty)
      m_queue_cond.notify_one();
  }

  T wait_pop() {
    boost::mutex::scoped_lock lock(m_queue_mutex);
    while(m_queue.empty())
      m_queue_cond.wait(lock);
    T val = m_queue.front();
    m_queue.pop_front();
    return val;
  }

  bool try_pop(T& val) {
    boost::mutex::scoped_lock lock(m_queue_mutex);
    if(m_queue.empty())
      return false;
    val = m_queue.front();
    m_queue.pop_front();
    return true;
  }
private:
  std::deque<T> m_queue;
  boost::mutex m_queue_mutex;
  boost::condition_variable m_queue_cond;
};


#endif // QNT_CORE_UTIL_QUEUE_HPP
