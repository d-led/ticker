#pragma once

#include <functional>
#include <ppl.h>
#include <ppltasks.h>

template<typename tick = std::function<void()>>
class active_ticker
{
	tick tick_once;
	concurrency::cancellation_token_source cts;
	concurrency::cancellation_token token;
	concurrency::task<void> t;
public:
	active_ticker(tick fun) :
		tick_once(fun),
		token(cts.get_token())
	{}
public:
	void start(unsigned interval)
	{
		cts = concurrency::cancellation_token_source();
		token = cts.get_token();
		t = create_task([&, interval]
		{
			while (true)
			{
				// Check for cancellation. 
				if (concurrency::is_task_cancellation_requested())
				{
					concurrency::cancel_current_task();
				}
				else
				{
					concurrency::wait(interval);
					tick_once();
				}
			}
		}, token);
	}

	void stop()
	{
		cts.cancel();
	}
};
