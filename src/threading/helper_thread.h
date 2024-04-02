#include <exception>
#include <functional>
#include "debug.h"
#include "threading/thread.h"

// A thread class that runs a lambda and provides a way to forward exceptions.
class HelperThread : public Thread
{
public:
	HelperThread(const std::string &name, const std::function<void()> &fn) :
		Thread(name),
		m_fn(fn)
	{}

	void rethrowException()
	{
		sanity_check(!isRunning());
		if (m_exptr)
			std::rethrow_exception(m_exptr);
	}

private:
	std::function<void()> m_fn;
	std::exception_ptr m_exptr;

	void *run()
	{
		try {
			m_fn();
		} catch(...) {
			m_exptr = std::current_exception();
		}
		return nullptr;
	};
};
