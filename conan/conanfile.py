from conans import ConanFile, CMake
from conan.tools.cmake import CMakeDeps

class QuetzalCoaTLConan(ConanFile):
    name = "quetzal-CoaTL"
    version = "0.1"
    settings = "os", "compiler", "arch", "build_type"
    exports_sources = "include/*", "CMakeLists.txt", "test/*", "cmake/*", "docs/*"
    no_copy_source = True
    generators = "cmake", "CMakeToolchain", "CMakeDeps"
    # zlib overriden because conflict on ubuntu 22.04 (gh-actions)
    requires = "boost/1.80.0", "gdal/3.5.2", "zlib/1.2.13"
    tool_requires = "cmake/3.22.0", "doxygen/1.9.2"

    def generate(self):
        cmake = CMakeDeps(self)
        # generate the config files for the tool require
        cmake.build_context_activated = ["cmake/3.22.0","doxygen/1.9.2"]
        cmake.generate()

    def build(self): # this is not building a library, just tests
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test(output_on_failure=True)

    def package(self):
        self.copy("*.h")

    def package_id(self):
        self.info.clear()
