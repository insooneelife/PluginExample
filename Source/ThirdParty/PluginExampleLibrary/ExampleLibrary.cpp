#if defined _WIN32 || defined _WIN64
    #include <Windows.h>
    #include <time.h>
    #include <sstream>
    #include <string>
    #include <algorithm>
    #define EXAMPLELIBRARY_EXPORT __declspec(dllexport)
#else
    #include <stdio.h>
#endif

#ifndef EXAMPLELIBRARY_EXPORT
    #define EXAMPLELIBRARY_EXPORT
#endif

#include "ExampleLibrary.h"

EXAMPLELIBRARY_EXPORT std::wstring ExampleLibraryFunction(const std::wstring& onnx_path)
{ 
    return L"";
}


std::wstring OnnxModelExploreTest::print_shape(const std::vector<int64_t>& v)
{
    std::wstringstream ss(L"");
    for (size_t i = 0; i < v.size() - 1; i++)
        ss << v[i] << "x";
    ss << v[v.size() - 1];
    return ss.str();
}

int OnnxModelExploreTest::calculate_product(const std::vector<int64_t>& v)
{
    int total = 1;
    for (auto& i : v) total *= i;
    return total;
}

std::vector<const char*> OnnxModelExploreTest::get_input_names(const Ort::Session& session)
{
    std::vector<const char*> input_names(session.GetInputCount());
    Ort::AllocatorWithDefaultOptions allocator;

    for (int i = 0; i < session.GetInputCount(); ++i)
    {
        input_names[i] = session.GetInputName(i, allocator);
    }
    return input_names;
}

std::vector<const char*> OnnxModelExploreTest::get_output_names(const Ort::Session& session)
{
    std::vector<const char*> output_names(session.GetOutputCount());
    Ort::AllocatorWithDefaultOptions allocator;

    for (int i = 0; i < session.GetOutputCount(); ++i)
    {
        output_names[i] = session.GetOutputName(i, allocator);
    }
    return output_names;
}

std::vector<std::vector<int64_t>> OnnxModelExploreTest::get_input_shapes(const Ort::Session& session)
{
    std::vector<std::vector<int64_t>> input_shapes(session.GetInputCount());

    for (int i = 0; i < session.GetInputCount(); ++i)
    {
        const Ort::TypeInfo& type_info = session.GetInputTypeInfo(i);
        const Ort::TensorTypeAndShapeInfo& tensor_info = type_info.GetTensorTypeAndShapeInfo();
        input_shapes[i] = tensor_info.GetShape();
    }

    return input_shapes;
}

std::vector<std::vector<int64_t>> OnnxModelExploreTest::get_output_shapes(const Ort::Session& session)
{
    std::vector<std::vector<int64_t>> output_shapes(session.GetOutputCount());

    for (int i = 0; i < session.GetOutputCount(); ++i)
    {
        const Ort::TypeInfo& type_info = session.GetOutputTypeInfo(i);
        const Ort::TensorTypeAndShapeInfo& tensor_info = type_info.GetTensorTypeAndShapeInfo();
        output_shapes[i] = tensor_info.GetShape();
    }

    return output_shapes;
}

EXAMPLELIBRARY_EXPORT void OnnxModelExploreTest::main(const std::wstring& onnx_path, const wchar_t** msg)
{
    // onnxruntime setup
    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "example-model-explorer");
    Ort::SessionOptions session_options;
    Ort::Session session{ env, onnx_path.c_str(), session_options };  // access experimental components via the Experimental namespace

    Ort::MemoryInfo memory_info = Ort::MemoryInfo::CreateCpu(OrtDeviceAllocator, OrtMemTypeCPU);

    std::vector<const char*> input_names = get_input_names(session);
    std::vector<std::vector<int64_t> > input_shapes = get_input_shapes(session);

    std::wstringstream ss;

    ss << "Input Node Name/Shape (" << input_names.size() << "):" << std::endl;
    for (size_t i = 0; i < input_names.size(); i++) 
    {
        ss << "\t" << input_names[i] << " : " << print_shape(input_shapes[i]) << std::endl;
    }

    std::vector<const char*> output_names = get_output_names(session);
    std::vector<std::vector<int64_t> > output_shapes = get_output_shapes(session);

    ss << "Output Node Name/Shape (" << output_names.size() << "):" << std::endl;
    for (size_t i = 0; i < output_names.size(); i++) 
    {
        ss << "\t" << output_names[i] << " : " << print_shape(output_shapes[i]) << std::endl;
    }

    // Create a single Ort tensor of random numbers
    auto input_shape = input_shapes[0];
    int total_number_elements = calculate_product(input_shape);
    std::vector<float> input_tensor_values(total_number_elements);
    std::generate(input_tensor_values.begin(), input_tensor_values.end(), [&] { return rand() % 255; });  // generate random numbers in the range [0, 255]
    std::vector<Ort::Value> input_tensors;

    input_tensors.push_back(
        Ort::Value::CreateTensor<float>(
            memory_info, input_tensor_values.data(), input_tensor_values.size(), input_shape.data(), input_shape.size()));

    // double-check the dimensions of the input tensor
    ss << "\ninput_tensor shape: " << print_shape(input_tensors[0].GetTensorTypeAndShapeInfo().GetShape()) << std::endl;

    // pass data through model
    ss << "Running model...";
    
    std::vector<Ort::Value> output_tensors = session.Run(Ort::RunOptions{ nullptr },
        input_names.data(), input_tensors.data(), input_names.size(), output_names.data(), output_names.size());

    ss << "done" << std::endl;

    // double-check the dimensions of the output tensors
    // NOTE: the number of output tensors is equal to the number of output nodes specifed in the Run() call
    ss << "output_tensor_shape: " << print_shape(output_tensors[0].GetTensorTypeAndShapeInfo().GetShape()) << std::endl;

    *msg = ss.str().c_str();
}