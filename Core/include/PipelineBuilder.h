#pragma once

#include <string>

class PipelineBuilder
{
public:
	PipelineBuilder& SetVertexShader(std::string path);
	PipelineBuilder& SetFragmentShader(std::string path);
	PipelineBuilder& SetComputeShader(std::string path);
};