#include "stdafx.h"

using namespace concurrency;

class FrequencyMeter
{
	std::mt19937 gen;
	std::uniform_int_distribution<int> dis;
public:
	FrequencyMeter() :
		dis(0,3),
		gen(std::random_device()())
	{}

	int Hz()
	{
		auto r=rand();
		return 60+dis(gen);
	}
};

class active_ticker
{
	typedef std::function<void()> tick;
	tick tick_once;
	cancellation_token_source cts;
	cancellation_token token;
	task<void> t;
public:
	active_ticker(tick fun):
		tick_once(fun),
		token(cts.get_token())
	{}
public:
	void start(unsigned interval)
	{
		cts=cancellation_token_source();
		token=cts.get_token();
		t = create_task([&,interval]
		{
			while (true)
			{
				// Check for cancellation. 
				if (is_task_cancellation_requested())
				{
					cancel_current_task();
				}
				else 
				{
					wait(interval);
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

int main(int argc, char* argv[])
{
	FrequencyMeter FM;

	active_ticker measure=([&]{std::cout<<FM.Hz()<<std::endl;});
	measure.start(250);

	active_ticker ticker([]{std::cout<<"tick"<<std::endl;});
	ticker.start(500);


	// Wait for one second and then cancel the task.
	wait(2000);

	std::cout << "Canceling measurement ..." << std::endl;
	measure.stop();

	wait(2000);

	ticker.stop();

	std::cout << "Restarting measurement ..." << std::endl;
	measure.start(250);
	wait(2000);

	measure.stop();

	std::cout << "Done" << std::endl;

	return 0;
}