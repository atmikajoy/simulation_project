#include <iostream>
#include <thread>
#include <future>

struct factorial
{
	factorial(unsigned int n) : n(n) {}

	void operator() ()
	{
		while (n > 1)
		{
			result *= n;
			n -= 1;
		}
	}

	unsigned int n;
	unsigned long long result = 1;
};

unsigned long long fact(unsigned int n)
{
	unsigned long long result = 1;
	while (n > 1)
	{
		result *= n;
		n -= 1;
	}
	return result;
}

int main()
{
	factorial f(7);
	std::thread t( std::ref(f) );
	t.join();
	std::cout << f.result << '\n';

	auto fut = std::async(std::launch::async, fact, 7);
	fut.wait();	
	std::cout << fut.get() << '\n';
}
