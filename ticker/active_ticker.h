#pragma once

#include "ppl_include.h"

#include <functional>
#include <atomic>

template<typename tick = std::function<void()>>
class active_ticker
{
	std::atomic<bool> running;
	tick tick_once;
	pplx::cancellation_token_source cts;
	pplx::cancellation_token token;
	pplx::task<void> t;
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
		cts = pplx::cancellation_token_source();
		token = cts.get_token();
		t = create_task([this, interval]
		{
			while (running)
			{
				// Check for cancellation. 
				if (pplx::is_task_cancellation_requested())
				{
					running = false;
					pplx::cancel_current_task();
				}
				else
				{
					this->tick_once();
					pplx::wait(interval);						
				}
			}
		}, token);
	}

	void stop()
	{
		cts.cancel();
	}
};
