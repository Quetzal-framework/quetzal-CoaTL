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
                self.requires("boost/[>1.75 <1.80]")
                self.requires("gdal/[>=3.4.0]")
                self.requires("range-v3/0.12.0")
       if self.settings.os == "Linux":
            if self.settings.compiler == "gcc":
                self.requires("boost/[>1.75 <1.80]")
                self.requires("gdal/[>=3.4.0 <3.5.1]")
                self.requires("range-v3/0.12.0")
                self.requires("libtiff/4.5.1", override=True) # Version conflict: libgeotiff/1.7.1->libtiff/4.6.0, gdal/3.4.3->libtiff/4.5.1.
                self.requires("libdeflate/1.18", override=True) # Version conflict: libtiff/4.6.0->libdeflate/1.19, gdal/3.4.3->libdeflate/1.18.
                self.requires("proj/9.2.1", override=True) # Version conflict: libgeotiff/1.7.1->proj/9.3.0, gdal/3.4.3->proj/9.2.1.
                self.requires("sqlite3/3.42.0", override=True) # Version conflict: proj/9.3.0->sqlite3/3.43.2, gdal/3.4.3->sqlite3/3.42.0.
                self.requires("zlib/1.2.13", override=True) # Version conflict: gdal/3.4.3->zlib/1.2.13, quetzal-CoaTL/3.1.0->zlib/1.3.