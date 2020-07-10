////////////////////////////////////////////////
//                                            //
//           Asynchronous API Test            //
//                                            //
////////////////////////////////////////////////

#include <iostream>
#include "NxAsync.h"

class CPUDevice
{
public:
	CPUDevice()
	{
	}

	~CPUDevice()
	{
	}

	void uploadResource1()
	{
		std::cout << __FUNCTION__ << std::endl;
	}

	void uploadResource2()
	{
		std::cout << __FUNCTION__ << std::endl;
	}
};

class GPUDevice
{
protected:
	int passIndex;

public:
	GPUDevice()
		: passIndex(0)
	{
	}

	~GPUDevice()
	{
	}

	void setup()
	{
		std::cout << __FUNCTION__ << std::endl;
	}

	bool getNewJobs()
	{
		std::cout << __FUNCTION__ << std::endl;

		++passIndex;
		if (passIndex >= 4)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	void computeStep1()
	{
		std::cout << __FUNCTION__ << std::endl;
	}

	void computeStep2()
	{
		std::cout << __FUNCTION__ << std::endl;
	}

	void computeStep3()
	{
		std::cout << __FUNCTION__ << std::endl;
	}
};

void runSerial()
{
	CPUDevice CPU;
	GPUDevice GPU;

	GPU.setup();

	do
	{
		GPU.computeStep1();

		CPU.uploadResource1();

		GPU.computeStep2();

		GPU.computeStep3();

		CPU.uploadResource2();
	}
	while (GPU.getNewJobs());
}

void runParallel()
{
	CPUDevice CPU;
	GPUDevice GPU;

	TimeSlot slot0;
	bool hasNewJobs = false;
	slot0.add([&]()
	{
		GPU.setup();
		GPU.computeStep1();
		CPU.uploadResource1();
		GPU.computeStep2();
		hasNewJobs = GPU.getNewJobs();
	});
	slot0.wait([&]()
	{
		if (hasNewJobs)
		{
			GPU.computeStep1();
		}

		TimeSlot slot1;
		slot1.add([&]()
		{
			GPU.computeStep3();
		});
		if (hasNewJobs)
		{
			slot1.add([&]()
			{
				CPU.uploadResource1();
			});
		}
		slot1.wait([&]()
		{
			TimeSlot slot2;
			slot2.add([&]()
			{
				CPU.uploadResource2();
			});
			if (hasNewJobs)
			{
				slot2.add([&]()
				{
					GPU.computeStep2();
					hasNewJobs = GPU.getNewJobs();
				});
			}
			slot2.wait([&]()
			{
				if (hasNewJobs)
				{
					slot0.call();
				}
			});
		});
	});
}

int main(int argc, char * const argv[])
{
	std::cout << "Testing serial..." << std::endl;

	runSerial();

	std::cout << "Testing parallel..." << std::endl;

	runParallel();

	return 0;
}
