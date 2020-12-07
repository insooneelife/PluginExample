#if defined _WIN32 || defined _WIN64
#define EXAMPLELIBRARY_IMPORT __declspec(dllimport)
#include <string>
#include <vector>
#include <onnxruntime_cxx_api.h>
#elif defined __linux__
#define EXAMPLELIBRARY_IMPORT __attribute__((visibility("default")))
#else
#define EXAMPLELIBRARY_IMPORT
#endif

EXAMPLELIBRARY_IMPORT std::wstring ExampleLibraryFunction(const std::wstring& onnx_path);

EXAMPLELIBRARY_IMPORT class OnnxModelExploreTest
{
public:
    static std::wstring print_shape(const std::vector<int64_t>& v);

    static int calculate_product(const std::vector<int64_t>& v);

    static std::vector<const char*> get_input_names(const Ort::Session& session);

    static std::vector<const char*> get_output_names(const Ort::Session& session);

    static std::vector<std::vector<int64_t>> get_input_shapes(const Ort::Session& session);

    static std::vector<std::vector<int64_t>> get_output_shapes(const Ort::Session& session);

    EXAMPLELIBRARY_IMPORT static void main(const std::wstring& onnx_path, const wchar_t** msg);
};
