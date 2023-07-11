#include "profiler_manager.h"

std::vector<Profiler*> ProfilerManager::profilers;

void ProfilerManager::CreateProfiler(ProfilerClass profilerClass, std::string label)
{
	Profiler* newProfiler = new Profiler(profilerClass, label);
	profilers.push_back(newProfiler);
}

void ProfilerManager::CreateSubProfiler(std::string ownerLabel, std::string label)
{
	for (int i = 0; i < profilers.size(); i++)
	{
		if (profilers[i]->getLabel() == ownerLabel)
		{
			Profiler* subProfiler = profilers[i]->CreateSubProfiler(label);
			profilers.push_back(subProfiler);
		}
	}
}

void ProfilerManager::EndProfiler(std::string label)
{
	for (int i = 0; i < profilers.size(); i++)
	{
		if (profilers[i]->getLabel() == label)
		{
			profilers[i]->EndProfiler();
			return;
		}
	}
}

void ProfilerManager::PrintProfiler(std::string label)
{
	for (int i = 0; i < profilers.size(); i++)
	{
		if (profilers[i]->getLabel() == label)
		{
			profilers[i]->print();
			return;
		}
	}
}

void ProfilerManager::ClearEndedProfilers()
{
	for (int i = profilers.size() - 1; i >= 0; i--)
	{
		if (profilers[i] == nullptr || profilers[i]->isEnded())
		{
			Profiler* profiler = profilers[i];
			profilers.erase(std::next(profilers.begin(), i));
		}
	}
}