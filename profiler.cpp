#include "profiler.h"

Profiler::Profiler(ProfilerClass profilerClass, std::string label)
{
	this->label = label;
	this->profilerClass = profilerClass;

	start = glfwGetTime();
}

Profiler::Profiler(ProfilerClass profilerClass, std::string label, std::string ownerLabel) : Profiler(profilerClass, label)
{
	this->ownerLabel = ownerLabel;
}

Profiler::~Profiler()
{
	for (int i = 0; i < subprofilers.size(); i++)
	{
		if (subprofilers[i] != nullptr) 
			delete subprofilers[0];
		subprofilers.erase(subprofilers.begin());
	}
}

// Creates a profiler child with label
Profiler* Profiler::CreateSubProfiler(std::string label)
{
	Profiler* newProfiler = new Profiler(profilerClass, label, this->label);
	subprofilers.push_back(newProfiler);
	return newProfiler;
}

// Prints out all of the data collected by the profiler
void Profiler::print()
{
	if (profilerEnded)
	{
		float mainClockDuration = getClockDuration();

		std::vector<std::string> subProfilerOutputs;
		for (auto profiler : subprofilers)
		{
			float subProfilerDuration = profiler->getClockDuration();
			float percentage = subProfilerDuration / mainClockDuration;

			std::string output = " - Subprofiler \'" + profiler->label + "\' ran for " + std::to_string(percentage * 100) + "% of the profiler time.\n";
			subProfilerOutputs.push_back(output);
		}

		std::cout << "Profiler \'" << label << "\':" << std::to_string(mainClockDuration) << ".\n";
		for (auto output : subProfilerOutputs)
			std::cout << output;
		std::cout << std::endl;
	}
	else
	{
		std::cout << "Profiler must be ended before printing.\n";
	}
}

// Ends profiler and its children
void Profiler::EndProfiler()
{
	if (!profilerEnded) {
		for (auto profiler : subprofilers)
			profiler->EndProfiler();

		finish = glfwGetTime();

		profilerEnded = true;
	}
}

void Profiler::EndSubProfiler(std::string sublabel)
{
	for (auto profiler : subprofilers)
	{
		if (profiler->label == sublabel)
		{
			profiler->EndProfiler();
			return;
		}
	}
}

std::string Profiler::getLabel() { return label; }
std::string Profiler::getOwnerLabel() { return ownerLabel; }
bool Profiler::isEnded() { return profilerEnded; }