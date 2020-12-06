#if defined _WIN32 || defined _WIN64
    #include <Windows.h>
    #include <onnxruntime_cxx_api.h>
    #include <time.h>
    #include <sstream>
    #include <string>
    #define EXAMPLELIBRARY_EXPORT __declspec(dllexport)
#else
    #include <stdio.h>
#endif

#ifndef EXAMPLELIBRARY_EXPORT
    #define EXAMPLELIBRARY_EXPORT
#endif

EXAMPLELIBRARY_EXPORT std::wstring ExampleLibraryFunction(const std::wstring& onnx_path)
{
    const size_t BZ = 10;
    const size_t T = 32;

    const size_t XDIM = 15;
    const size_t YDIM = 138;
    const OrtApi* g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    const size_t SEQ = (512 - 32);
    const size_t N_EVAL = 32;

    clock_t etime = 0;

    Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "test");
    Ort::SessionOptions session_options;

    session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_BASIC);
    Ort::Session session{ env, onnx_path.c_str(),  session_options };

    size_t num_input_nodes = session.GetInputCount();
    Ort::AllocatorWithDefaultOptions allocator;

    std::vector<const char*> input_node_names(num_input_nodes);
    std::vector<int64_t> input_node_dims;

    std::wstringstream ss;
    ss << "Number of inputs size : " << input_node_names.size() << std::endl;

    for (size_t i = 0; i < num_input_nodes; ++i)
    {
        input_node_names[i] = session.GetInputName(i, allocator);
        ss << "Input : " << i << " name : " << input_node_names[i] << std::endl;

        const Ort::TypeInfo& type_info = session.GetInputTypeInfo(i);
        const Ort::TensorTypeAndShapeInfo& tensor_info = type_info.GetTensorTypeAndShapeInfo();

        size_t num_dims = tensor_info.GetDimensionsCount();
        input_node_dims.resize(num_dims);

        ss << "Input : " << i << " dims size : " << input_node_dims[i] << std::endl;

        tensor_info.GetDimensions(input_node_dims.data(), input_node_dims.size());

        for (size_t j = 0; j < num_dims; ++j)
        {
            ss << "Input : " << i << " dim " << j << "=" << input_node_dims[i] << std::endl;
        }
    }

    std::array<const char*, 1> output_node_names = { "y_pred" };
    std::vector<float> xs(XDIM * T * BZ);
    std::vector<float> ys(YDIM * T * BZ);
    std::array<int64_t, 3> xs_node_dims = { BZ, T, XDIM };
    std::array<int64_t, 3> ys_node_dims = { BZ, T, YDIM };

    for (size_t n1 = 0; n1 < N_EVAL; ++n1)
    {
        for (size_t n2 = 0; n2 < SEQ; ++n2)
        {
            for (size_t i = 0; i < T; ++i)
            {
                xs[i] = ((float)(rand() % 1000)) / 1000.0f - 0.5f;
                ys[i] = ((float)(rand() % 1000)) / 1000.0f - 0.5f;
            }

            clock_t start = clock();
            float* probs;

            Ort::MemoryInfo memory_info =
                Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);

            std::array<Ort::Value, 2> inputs
            {
                Ort::Value::CreateTensor<float>(memory_info, xs.data(), xs.size(), xs_node_dims.data(), xs_node_dims.size()),
                Ort::Value::CreateTensor<float>(memory_info, ys.data(), ys.size(), ys_node_dims.data(), ys_node_dims.size())
            };

            Ort::Value output{ nullptr };
            session.Run(
                Ort::RunOptions{ nullptr }, input_node_names.data(), inputs.data(), 2, output_node_names.data(), &output, 1);
            probs = output.GetTensorMutableData<float>();

            etime += clock() - start;

            for (size_t c = 0; c < YDIM; ++c)
            {
                for (size_t t = 0; t < T; ++t)
                {
                    for (size_t bz = 0; bz < BZ; ++bz)
                    {
                        //int index = (BZ * T) *c + (BZ)*t + bz;
                        int index = bz * (T * YDIM) + t * (YDIM)+c;
                        float value = probs[index];
                    }
                }
            }
        }

        ss << ((double)etime / CLOCKS_PER_SEC / (n1 + 1) / SEQ) << std::endl;
    }

    ss << "Done!" << std::endl;
    ss << ((double)etime / CLOCKS_PER_SEC / N_EVAL / SEQ) << std::endl;

    return ss.str();
}