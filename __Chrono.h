// Time measuring class

/*
	Usage example:

		Chrono timer;

		SOME_TIME_CONSUMING_FUNCTION();

		timer.end();

		std::cout << " -- Started at: "   << timer.getStartTime();
		std::cout << " -- Finished at: "  << timer.getEndTime();
		std::cout << " -- Elapsed time: " << timer.getElapsedTime() << std::endl;

		// and again:
		timer.start();
		...
		timer.end();
*/

#pragma once
#ifndef _CHRONO_H_
#define _CHRONO_H_

#include <chrono>
#include <ctime>

// -----------------------------------------------------------------------------------------------

class Chrono {

	public:

		Chrono()
		{
			start();
		}

		void start()
		{
			t = std::chrono::system_clock::now();
		}

		void end()
		{
			t_end  = std::chrono::system_clock::now();
			elapsed_seconds = t_end - t;
		}

		double getElapsedTime() const
		{
			return elapsed_seconds.count();
		}

		const char* getStartTime()
		{
			start_time = std::chrono::system_clock::to_time_t(t);

			ctime_s(buf, 256, &start_time);
			return buf;
		}

		const char* getEndTime()
		{
			end_time = std::chrono::system_clock::to_time_t(t_end);

			ctime_s(buf, 256, &end_time);
			return buf;
		}

static	void Sleep(const size_t mS)
		{
			std::chrono::milliseconds timespan(mS);
			std::this_thread::sleep_for(timespan);
		}

	private:

		char									buf[256];
		std::chrono::system_clock::time_point	t, t_end;
		std::chrono::duration<double>			elapsed_seconds;
		std::time_t								start_time, end_time;

};

// -----------------------------------------------------------------------------------------------

#endif
