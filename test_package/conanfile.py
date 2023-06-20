from conans import ConanFile, CMake


class MylibTestConan(ConanFile):
    settings   = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    # No explicit requires of the top-level library, Conan does it by itself!

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        self.run("bin/unit-test")
