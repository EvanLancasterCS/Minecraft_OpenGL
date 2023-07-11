#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "shader.h"

enum class ProfilerClass
{
	ChunkGeneration
};

class Profiler
{
public:
	// Starts timer on creation
	Profiler(ProfilerClass profilerClass, std::string label);

	Profiler(ProfilerClass profilerClass, std::string label, std::string ownerLabel);

	~Profiler();

	// Creates a profiler child with label
	Profiler* CreateSubProfiler(std::string label);

	// Prints out all of the data collected by the profiler
	void print();

	// Ends profiler and its children
	void EndProfiler();

	void EndSubProfiler(std::string sublabel);

	std::string getLabel();
	std::string getOwnerLabel();
	bool isEnded();

private:

	double getClockDuration()
	{
		return finish - start;
	}

	ProfilerClass profilerClass;
	std::vector<Profiler*> subprofilers;
	std::string label;
	std::string ownerLabel = "";

	double start;
	double finish;

	bool profilerEnded = false;
};