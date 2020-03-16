#pragma once
#include "antenna-prereq.hpp"
#include "message.hpp"

namespace antenna
{
	class Queue;
	class IQueueDelegate
	{
	public:
		virtual void after_push(Queue& this_queue) noexcept = 0;
		virtual void after_pop(Queue& this_queue, bool pop_succeeded) noexcept = 0;
	};

	class Queue
	{
	public:
		Queue();
		~Queue() noexcept;

	public:
		void	push(Message&& node);
		bool	pop(Message& node);
		bool	pop_batch(std::vector<Message>& nodes, size_t max_size);
		bool	pop_all(std::vector<Message>& nodes);

		size_t	size() const noexcept;
		void	set_delegate(std::shared_ptr<IQueueDelegate> delegate) noexcept;

	protected:
		std::deque<Message>				_queue;
		std::mutex						_mtx;
		std::shared_ptr<IQueueDelegate>	_delegate;
	};
}

