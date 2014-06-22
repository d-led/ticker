#include "stdafx.h"

#include "frequency_meter.h"
#include "active_ticker.h"

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
}

int main(int argc, char* argv[])
{
	std::cout << "--- ppl ---" << std::endl;
	ppl_example();
	return 0;
}