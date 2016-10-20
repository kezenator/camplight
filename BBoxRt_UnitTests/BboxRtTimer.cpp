#include <unittest/UnitTest.h>

#include <bbox/rt/Proactor.h>
#include <bbox/rt/ServiceFolder.h>
#include <bbox/rt/Timer.h>

#include <bbox/debug/DebugProvider.h>
#include <bbox/debug/DebugTarget.h>

#include <bbox/Format.h>

#include <thread>

using namespace bbox::rt;
using Assert = unittest::Assert;

namespace
{
    class TestProvider: public bbox::debug::DebugProvider,
		                public bbox::debug::DebugTarget
    {
    public:
        TestProvider()
            : bbox::debug::DebugProvider()
			, bbox::debug::DebugTarget()
        {
        }

        void OutputToTarget(bbox::debug::DebugReport report) override
        {
            std::string copy(report.Data());
            if (!copy.empty())
            {
                if (copy[copy.size() - 1] == '\n')
                {
                    copy.pop_back();
                }

                ::unittest::Log(copy);
            }
        }
    };
}

namespace BBoxRt_UnitTests
{
    UT_TEST_CLASS(TimerTests)
    {
    private:
        static void Verify(const std::vector<size_t> &counters, size_t i, size_t j, size_t k, const unittest::LineInfo &info)
        {
            Assert::AreEqual(counters[0], i, "1", info);
            Assert::AreEqual(counters[1], j, "2", info);
            Assert::AreEqual(counters[2], k, "3", info);
        }

    public:

        TimerTests()
        {
            UT_REGISTER_METHOD(TimerTest)
        }

        UT_TEST_METHOD(TimerTest)
        {
            TestProvider test_provider;
            Proactor proactor;
            ServiceFolder folder("folder", proactor);
            std::vector<std::unique_ptr<Timer>> timers;
            std::vector<size_t> counters;

            timers.resize(3);
            counters.resize(3);
            for (size_t i = 0; i < timers.size(); i++)
            {
                timers[i] = std::unique_ptr<Timer>(new Timer(bbox::Format("Timer-%d", i), folder));
                counters[i] = 0;

                timers[i]->SetHandler([&counters, i]() { counters[i]++; });
            }

            Proactor::UnitTestRunner runner(proactor);

            runner.Poll();

            timers[0]->StartPeriodic(TimeSpan::Milliseconds(1000));
            timers[1]->StartSingleShot(TimeSpan::Milliseconds(3000));
            timers[2]->StartPeriodic(TimeSpan::Milliseconds(2000));

            Verify(counters, 0, 0, 0, UT_LINE_INFO());

            runner.Poll();

            Verify(counters, 0, 0, 0, UT_LINE_INFO());

            // First, sleep for 500ms - expect nothing

            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            runner.Poll();
            Verify(counters, 0, 0, 0, UT_LINE_INFO());

            // Then sleep for 7 seconds and check results

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            runner.Poll();
            Verify(counters, 1, 0, 0, UT_LINE_INFO());

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            runner.Poll();
            Verify(counters, 2, 0, 1, UT_LINE_INFO());

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            runner.Poll();
            Verify(counters, 3, 1, 1, UT_LINE_INFO());

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            runner.Poll();
            Verify(counters, 4, 1, 2, UT_LINE_INFO());

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            runner.Poll();
            Verify(counters, 5, 1, 2, UT_LINE_INFO());

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            runner.Poll();
            Verify(counters, 6, 1, 3, UT_LINE_INFO());

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            runner.Poll();
            Verify(counters, 7, 1, 3, UT_LINE_INFO());

            // Finally, shutdown

            for (size_t i = 0; i < timers.size(); ++i)
                timers[i]->Cancel();

            proactor.RequestStop();
            runner.Run();
        }

    };

    UT_REGISTER_CLASS(TimerTests);
}
