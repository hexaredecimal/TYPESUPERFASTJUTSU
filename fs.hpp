
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace Fs {

namespace fs = std::filesystem;

bool getImagesInPath(std::string path, std::vector<std::string>& list, std::string ext) {
  try {
    for (const auto &entry : fs::directory_iterator(path)) {
      if (entry.is_regular_file() && entry.path().extension() == ext) {
        auto _path = entry.path().string();
        list.push_back(_path);
      }
    }
    return true;
  } catch (const fs::filesystem_error &e) {
    std::cerr << "Filesystem error: " << e.what() << std::endl;
    return false;
  } catch (const std::exception &e) {
    std::cerr << "General error: " << e.what() << std::endl;
    return false;
  }
}

} // namespace Fs