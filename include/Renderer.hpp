#include <vector>
#include <string>

namespace render{
    void ppm(const std::vector<double>& noise, const unsigned width, const unsigned height, const std::string& filename = "output.ppm");
}