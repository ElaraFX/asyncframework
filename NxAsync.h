///////////////////////////////////////////////////////////////////
//                                                               //
//             Asynchronous APIs for Task Management             //
//                                                               //
// Author : Elvic Liang                                          //
// Date   : 2020/6/29                                            //
//                                                               //
///////////////////////////////////////////////////////////////////

#pragma once

#include <atomic>
#include <cassert>
#include <vector>
#include <functional>
#include <future>

class TimeSlot
{
protected:
	std::vector<std::function<void()> > m_functors;
	std::function<void()> m_waitFunctor;
	std::vector<std::future<void> > m_promises;

public:
	TimeSlot()
	{
	}

	~TimeSlot()
	{
	}

	template <typename FUNC>
	void add(FUNC func)
	{
		m_functors.push_back(func);
	}

	void wait()
	{
		m_promises.resize(m_functors.size());

		for (size_t i = 0; i < m_functors.size(); ++i)
		{
			m_promises[i] = std::async(std::launch::async, m_functors[i]);
		}

		for (size_t i = 0; i < m_functors.size(); ++i)
		{
			m_promises[i].wait();
		}
	}

	template <typename FUNC>
	void wait(FUNC func)
	{
		m_waitFunctor = func;
		wait();
		m_waitFunctor();
	}

	void call()
	{
		m_waitFunctor();
	}
};

class Pipeline
{
protected:
	std::vector<TimeSlot *> m_timeSlots;

public:
	Pipeline()
	{
	}

	~Pipeline()
	{
		for (size_t i = 0; i < m_timeSlots.size(); ++i)
		{
			delete m_timeSlots[i];
		}
	}

	TimeSlot & newTimeSlot()
	{
		TimeSlot *newSlot = new TimeSlot;
		m_timeSlots.push_back(newSlot);
		return *newSlot;
	}
};
