#include "stdafx.h"

#include "frequency_meter.h"

#include <rxcpp/rx.hpp>

namespace {

    static void sleep (int milliseconds) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    void rxcpp_example() {
        FrequencyMeter FM;

        std::atomic<long> pending(2);

        // schedule everything on the same event loop thread.
        auto scheduler = rxcpp::schedulers::make_same_worker(rxcpp::schedulers::make_event_loop().create_worker());
        auto coordination = rxcpp::identity_one_worker(scheduler);

        auto measure = rxcpp::observable<>::interval(
            scheduler.now() + std::chrono::milliseconds(250),
            std::chrono::milliseconds(250),
            coordination);

        auto measure_subscription = measure
            .subscribe([&FM](int val) {
                std::cout << FM.Hz() << std::endl;
            });
        
        auto ticker = rxcpp::observable<>::interval(
            scheduler.now() + std::chrono::milliseconds(500),
            std::chrono::milliseconds(500),
            coordination);

        ticker
            .take(10)
            .subscribe([](int val) {
                std::cout << "tick " << val << std::endl;
            },[&](){
                --pending; // take completed the ticker
            });

        // schedule the cout on the same worker to keep it from merging with the other cout calls.
        scheduler.create_worker().schedule(scheduler.now() + std::chrono::seconds(2), 
            [&](const rxcpp::schedulers::schedulable&) {
                std::cout << "Canceling measurement ..." << std::endl;
                measure_subscription.unsubscribe(); // cancel measurement
                --pending; // signal measurement canceled
            });

        while (pending > 0) {
            sleep(1000); // wait for ticker and measure to finish
        }
    }
}

int main()
{
    rxcpp_example();
    return 0;
}
