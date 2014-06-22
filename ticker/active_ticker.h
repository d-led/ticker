#pragma once

#include <functional>
#include <ppl.h>
#include <ppltasks.h>
#include <atomic>

template<typename tick = std::function<void()>>
class active_ticker
{
	std::atomic<bool> running;
	tick tick_once;
	concurrency::cancellation_token_source cts;
	concurrency::cancellation_token token;
	concurrency::task<void> t;
public:
	active_ticker(tick fun) :
		tick_once(fun),
		token(cts.get_token())
	{}

	~active_ticker() {
		this->stop();
	}

public:
	void start(unsigned interval)
	{
		running = true;
		cts = concurrency::cancellation_token_source();
		token = cts.get_token();
		t = create_task([this, interval]
		{
			while (running)
			{
				// Check for cancellation. 
				if (concurrency::is_task_cancellation_requested())
				{
					running = false;
					concurrency::cancel_current_task();
				}
				else
				{
					this->tick_once();
					concurrency::wait(interval);						
				}
			}
		}, token);
	}

	void stop()
	{
		cts.cancel();
	}
};
