#include "queue.hpp"

using namespace std;
using namespace antenna;

Queue::Queue()
{}

Queue::~Queue()
{}

void Queue::push(Message&& node)
{
	{
		std::unique_lock<std::mutex> lock(_mtx);
		_queue.push_back(std::move(node));
	}

	if (_delegate != nullptr)
	{
		_delegate->after_push(*this);
	}
}

bool Queue::pop(Message& node)
{
	bool res = false;
	{
		std::unique_lock<std::mutex> lock(_mtx);
		if (_queue.size() > 0)
		{
			node = std::move(_queue.front());
			_queue.pop_front();
			res = true;
		}
	}

	if (_delegate != nullptr)
	{
		_delegate->after_pop(*this, res);
	}

	return res;
}

bool Queue::pop_batch(std::vector<Message>& nodes, size_t max_size)
{
	bool res = false;
	{
		std::unique_lock<std::mutex> lock(_mtx);
		size_t poplen = _queue.size() < max_size ? _queue.size() : max_size;
		if (poplen > 0)
		{
			nodes.resize(poplen);
			for (size_t i = 0; i < poplen; ++i)
			{
				nodes[i] = std::move(_queue.front());
				_queue.pop_front();
			}
			res = true;
		}
	}

	if (_delegate != nullptr)
	{
		_delegate->after_pop(*this, res);
	}

	return res;
}

bool Queue::pop_all(std::vector<Message>& nodes)
{
	return pop_batch(nodes, _queue.size());
}

size_t Queue::size() const noexcept
{
	return _queue.size();
}

void Queue::set_delegate(std::shared_ptr<IQueueDelegate> delegate) noexcept
{
	_delegate = delegate;
}

