#include "stdafx.h"

#include "frequency_meter.h"
#include "active_ticker.h"
#include <cpprx/rx.hpp>
#include <chrono>

namespace {
	void ppl_example() {
		using concurrency::wait;

		FrequencyMeter FM;

		active_ticker<> measure([&]{std::cout << FM.Hz() << std::endl; });
		measure.start(250);

		active_ticker<> ticker([]{std::cout << "tick" << std::endl; });
		ticker.start(500);

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
	}

	void rxcpp_example() {
		FrequencyMeter FM;
		auto scheduler = std::make_shared<rxcpp::EventLoopScheduler>();
		auto measure = rxcpp::Interval(std::chrono::milliseconds(250),scheduler);
		auto sleep = [&scheduler](int milliseconds) {
			//rxcpp::from(rxcpp::Interval(std::chrono::milliseconds(milliseconds), scheduler))
			//	.take(1)
			//	.for_each([](int){});

			//concurrency::wait(milliseconds);

			std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
		};

		auto measure_subscription = rxcpp::from(measure)
			.subscribe([&FM](int val) {
				std::cout << FM.Hz() << std::endl;
			});
		
		auto ticker = rxcpp::Interval(std::chrono::milliseconds(500), scheduler);
		rxcpp::from(ticker)
			.take(10)
			.subscribe([](int val) {
				std::cout << "tick " << val << std::endl;
			});


		sleep(2000);
		std::cout << "Canceling measurement ..." << std::endl;
		measure_subscription.Dispose(); // cancel measurement

		sleep(6000); // wait for ticker to finish
	}
}

int main(int argc, char* argv[])
{
	std::cout << "--- ppl ---" << std::endl;
	ppl_example();

	std::cout << "--- rxcpp ---" << std::endl;
	rxcpp_example();
	return 0;
}