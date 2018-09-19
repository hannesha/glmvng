#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <chrono>
#include <atomic>
#include <vector>
#include <memory>
#include <cmath>
#include <csignal>

using namespace std::chrono_literals;

std::atomic_bool g_running(true);

void int_handler(int signal) {
	g_running = false;
}

struct fft_sync {
	std::mutex result_mut;
	std::condition_variable result_cv;
	std::atomic_int processed;
};

std::mutex o_mut;

void print_sync(const char* str) {
	std::lock_guard<std::mutex> lock(o_mut);
	std::cout << str << std::endl;
}

struct Semaphore {
	std::mutex mutex_;
	std::condition_variable cv_;
	int count = 0;

	template <typename Predicate>
	void wait(int expected_val, Predicate pred) {
		std::unique_lock<std::mutex> lock(mutex_);

		while(pred() && count < expected_val) {
			cv_.wait(lock);
		}

		count--;
	}

	void wait_all(int expected_val) {
		std::unique_lock<std::mutex> lock(mutex_);
		cv_.wait(lock, [&] { return count >= expected_val; });
		count = 0;
	}

	template <class Rep, class Period>
	void wait_all_for(int expected_val, const std::chrono::duration<Rep, Period>& dur) {
		std::unique_lock<std::mutex> lock(mutex_);
		cv_.wait_for(lock, dur, [&] { return count >= expected_val; });
		count = 0;
	}

	void notify() {
		std::unique_lock<std::mutex> lock(mutex_);
		count++;
		cv_.notify_one();
	}

	void notify_all(int val) {
		std::unique_lock<std::mutex> lock(mutex_);
		count = val;
		cv_.notify_all();
	}
};

struct fft_thread {
	fft_thread(Semaphore& s_stop): s2(s_stop), running(true),
		th_fft([this] { fft_thread_loop(); })
	{}

	~fft_thread() {
		print_sync("terminating");
		running = false;
		s1.notify();

		if(th_fft.joinable()) { th_fft.join(); }

		print_sync("terminated");
	}

	void fft_thread_loop() {
		std::vector<long> test(1000);

		while(true) {

			s1.wait(1, [&] { return (bool)running; });

			if(!running) {
				return;
			}

			//print_sync("starting work");

			//for(int i = 0; i < test.size(); i++){
			//	test[i] = (long)std::pow(13, i) % 17L;
			//}

			//print_sync("done");
			s2.notify();
		}
	}

	void new_job() {
		s1.notify();
		//print_sync("new_job");
	}

private:
	Semaphore s1;
	Semaphore& s2; // signals main thread
	std::atomic_bool running;
	std::thread th_fft;
};

int main() {
	std::signal(SIGINT, int_handler);

	Semaphore s_stop;

	const int nthreads = 40;
	std::vector<std::unique_ptr<fft_thread>> threads;

	for(int i = 0; i < nthreads; i++) {
		threads.emplace_back(new fft_thread(s_stop));
	}

	for(int i = 0; i < 100000 && g_running; i++) {
		std::cout << i << std::endl;

		for(auto& th : threads) {
			th->new_job();
		}


		s_stop.wait_all_for(nthreads, 3s);
		print_sync("finished round");
	}

	return 0;
}
