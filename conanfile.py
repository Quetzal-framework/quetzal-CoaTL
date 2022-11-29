from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, CMake
from conan.tools.files import copy
import os

class QuetzalCoaTLConan(ConanFile):

    name            = "quetzal-CoaTL"
    url             = "https://github.com/Quetzal-framework/quetzal-CoaTL"
    license         = "GPLv3"
    description     = "Quetzal Coalescence Template Library"
    version         = "0.1"

    settings        = "os", "compiler", "arch", "build_type"
    exports_sources = "include/*", "CMakeLists.txt", "test/*", "cmake/*", "docs/*"
    no_copy_source  = True

    requires        = "boost/[>1.75 <1.80]", "gdal/[>=3.4.0]"     # on Macos Monterey clang 13
    #requires       = "boost/1.80.0", "gdal/3.5.2", "zlib/1.2.13" # on Ubuntu 22.04 for gh-actions

    def package(self):
        copy(self, "*.h*",
            src=os.path.join(self.source_folder, "include"),
            dst=os.path.join(self.package_folder, "include"))

    def package_id(self):
        self.info.clear()
