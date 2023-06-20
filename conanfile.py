from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMakeDeps, CMake
from conan.tools.files import copy
import os

class QuetzalCoaTLConan(ConanFile):

    name            = "quetzal-CoaTL"
    url             = "https://github.com/Quetzal-framework/quetzal-CoaTL"
    license         = "GPLv3"
    description     = "Coalescence library for C++"
    version         = "3.1.0"

    settings        = "os", "compiler", "arch", "build_type"
    exports_sources = "src/*", "CMakeLists.txt", "test/*", "cmake/*", "docs/*"
    no_copy_source  = True
    build_policy    = "missing"
    #requires        = "boost/[>1.75 <1.80]", "gdal/[>=3.4.0]", "range-v3/0.12.0"     # on Macos Monterey clang 13
    #requires       = "boost/1.80.0", "gdal/3.5.2", "zlib/1.2.13" # on Ubuntu 22.04 for gh-actions

    def generate(self):
        tc = CMakeToolchain(self)
        tc.generate()
        deps = CMakeDeps(self)
        deps.generate()

    def build(self): # this is not building a library, just tests
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        cmake.test()

    def package(self):
        copy(self, "*.h*",
            src=os.path.join(self.source_folder, "include"),
            dst=os.path.join(self.package_folder, "include"))

    def package_id(self):
        self.info.clear()

    def requirements(self):
       if self.settings.os == "Macos":
            if self.settings.compiler == "apple-clang":
                self.requires("boost/[>1.75 <1.80")
                self.requires("gdal/[>=3.4.0]")
                self.requires("range-v3/0.12.0")
       if self.settings.os == "Linux":
            if self.settings.compiler == "gcc":
                self.requires("boost/1.80.0")
                self.requires("gdal/3.5.2")
                self.requires("range-v3/0.12.0")                
                self.requires("zlib/1.2.13", override=True)