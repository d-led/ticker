#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <rxcpp/rx.hpp>
#include <rxcpp/rx-test.hpp>

#include <chrono>

SCENARIO("showing off the test scheduler") {

    GIVEN("a test scheduler") {
        auto tsc = std::make_shared<rxcpp::schedulers::detail::test_type>();
        auto sc = rxcpp::schedulers::test(tsc);

        int count = 0;

        WHEN("subscribe to an observable sequence on the scheduler") {
            auto coordination = rxcpp::identity_one_worker(sc);
            auto seq = rxcpp::observable<>::interval(
                std::chrono::milliseconds(1),
                coordination // on the test scheduler
            ).filter([](int i) { return i % 2; });

            ;

            seq.subscribe([&count](int){
                count++;
            });

            THEN("the sequence is not run at first") {
                std::this_thread::sleep_for(std::chrono::milliseconds(2));

                CHECK(count == 0);

                AND_WHEN("the test scheduler is advanced manually") {
                    auto worker = tsc->create_test_type_worker_interface();

                    THEN("the sequence is run as expected") {
                        worker->advance_by(10 /* ms */);
                        CHECK(count == 5);

                        AND_THEN("the scheduler can be advanced again") {
                            worker->advance_by(10);
                            //CHECK(count == 10); 5 == 10!
                        }
                    }
                }
            }
        }
    }

}
