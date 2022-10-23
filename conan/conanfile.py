from conans import ConanFile, CMake

class QuetzalCoaTLConan(ConanFile):
    name = "quetzal-CoaTL"
    version = "0.1"
    settings = "os", "compiler", "arch", "build_type"
    exports_sources = "include/*", "CMakeLists.txt", "test/*", "cmake/*", "docs/*"
    no_copy_source = True
    generators = "cmake", "CMakeToolchain", "CMakeDeps"
    requires = "boost/[>=1.78.0]", "gdal/[>=3.4.3]","cmake/3.24.2", "doxygen/1.9.4"

    def build(self): # this is not building a library, just tests
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test(output_on_failure=True)

    def package(self):
        self.copy("*.h")

    def package_id(self):
        self.info.clear()
