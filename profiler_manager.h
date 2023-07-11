#pragma once
#include "profiler.h"

// Manages the creation and timing of profiling objects.
// Profilers begin when created, and end when EndProfiler is called.
// Subprofiles begin when created, and additionally end when their parent ends.
static class ProfilerManager
{
public:
	static void CreateProfiler(ProfilerClass profilerClass, std::string label);

	static void CreateSubProfiler(std::string ownerLabel, std::string label);

	static void EndProfiler(std::string label);

	static void PrintProfiler(std::string label);
	
	static void ClearEndedProfilers();

private:

	static std::vector<Profiler*> profilers;
};
