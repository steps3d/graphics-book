#ifndef	__TIMER__
#define	__TIMER__

#include	<chrono>

class	Timer
{
	std::chrono::steady_clock::time_point st;

public:
	Timer () 
	{
		start ();
	}

	~Timer() = default;

	void	start () 
	{
		st = std::chrono::steady_clock::now ();
	}

	double time () const
	{
		std::chrono::steady_clock::time_point curr = std::chrono::steady_clock::now ();

		return std::chrono::duration_cast<std::chrono::duration<double>>(curr-st).count ();
	}
};

#endif

