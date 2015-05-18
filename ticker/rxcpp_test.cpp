#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <rxcpp/rx.hpp>
#include <rxcpp/rx-test.hpp>

#include <chrono>

SCENARIO("showing off the test scheduler") {

    GIVEN("a test scheduler") {
        auto sc = rxcpp::schedulers::make_test();
        auto worker = sc.create_worker();
        auto test = rxcpp::identity_same_worker(worker);

        int count = 0;

        WHEN("subscribe to an observable sequence on the scheduler") {
            auto seq = rxcpp::observable<>::interval(
                std::chrono::milliseconds(1),
                test // on the test scheduler
            ).filter([](int i) { return i % 2; });

            seq.subscribe([&count](int){
                count++;
            });

            THEN("the sequence is not run at first") {
                worker.sleep(2 /* ms */);

                CHECK(count == 0);

                AND_WHEN("the test scheduler is advanced manually") {

                    THEN("the sequence is run as expected") {
                        worker.advance_by(8 /* ms */);
                        CHECK(count == 5);

                        AND_THEN("the scheduler can be advanced again") {
                            worker.advance_by(10);
                            CHECK(count == 10);
                        }
                    }
                }
            }
        }
    }

}
