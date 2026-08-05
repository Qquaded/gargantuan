GARGANTUAN_BUILD_DIRECTORY := "./build"
GARGANTUAN_BINARY := "./build/gargantuan"

# Runs the Gargantuan binary
run *args:
    -{{ GARGANTUAN_BINARY }} {{ args }}

# Configures the build directory
configure tracy="OFF" build_type="Debug":
    cmake -B {{ GARGANTUAN_BUILD_DIRECTORY }} -G Ninja \
        -DCMAKE_BUILD_TYPE={{ build_type }} \
        -DGARGANTUAN_TRACY={{ tracy }} \
        -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

# Builds the engine
build:
    cmake --build {{ GARGANTUAN_BUILD_DIRECTORY }} -j={{ num_cpus() }}

# Rebuilds the engine, then runs an example inside assets
fresh_example example *flags: build
    -{{ GARGANTUAN_BINARY }} --script=./assets/examples/{{ example }} {{ flags }}

# Rebuilds the engine, then runs the testbed project
fresh_testbed *flags: build
    -{{ GARGANTUAN_BINARY }} --project=./assets/testbed {{ flags }}

# Runs an example inside assets without rebuilding
run_example example *flags:
    -{{ GARGANTUAN_BINARY }} --script=./assets/examples/{{ example }} {{ flags }}

# Runs the testbed project without rebuilding
run_testbed *flags:
    -{{ GARGANTUAN_BINARY }} --project=./assets/testbed {{ flags }}

# Runs tests for the core library
test_core: build
    lest run core

# Runs tests for all libraries
test: test_core

# Builds the documentation site, then begins watching for changes
docs:
    cd docs && bunx astro dev
