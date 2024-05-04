#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

template <class Clock = std::chrono::steady_clock>
	requires(std::chrono::is_clock_v<Clock>)
class RecurringTimer {
public:
	RecurringTimer(
			std::function<void()> task,
			Clock::duration interval = std::chrono::seconds(1),
			bool auto_start = false) :
			task(task),
			interval(interval) {
		if (auto_start) {
			start();
		}
	}

	~RecurringTimer() {
		stop();
	}

	void start() {
		stop();
		thread = std::jthread([&](auto stoken) { run(stoken); });
	}

	void stop() {
		thread.request_stop();
		if (mutex.try_lock()) {
			// cv waiting, notify it
			cv.notify_one();
			mutex.unlock();
		}
		// otherwise let task finish
	}

	void set_interval(Clock::duration new_interval) {
		stop();
		interval = new_interval;
		next_run_time = Clock::now();
		start();
	}

private:
	void run(std::stop_token stoken) {
		std::unique_lock<std::mutex> lock(mutex);

		while (!stoken.stop_requested()) {
			next_run_time = std::max(next_run_time + interval, Clock::now());

			cv.wait_until(lock, next_run_time, [&]() { return Clock::now() >= next_run_time || stoken.stop_requested(); });

			if (stoken.stop_requested()) {
				return;
			}

			task();
		}
	}

	const std::function<void()> task;
	std::jthread thread;
	std::mutex mutex;
	std::condition_variable cv;

	// Should not need atomic since they are protected by mutex
	Clock::duration interval;
	Clock::time_point next_run_time;
};
