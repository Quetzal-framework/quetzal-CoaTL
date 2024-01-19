conan install conanfile.py \
    --profile:build=conan/profiles/linux-armv8-gcc12-release \
    --profile:host=conan/profiles/linux-armv8-gcc12-release \
    --build=missing \
    --output-folder=build

conan install conanfile.py \
    --profile:build=conan/profiles/linux-armv8-gcc12-debug \
    --profile:host=conan/profiles/linux-armv8-gcc12-debug \
    --build=missing \
    --output-folder=build