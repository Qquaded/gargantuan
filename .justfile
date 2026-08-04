GARGANTUAN_BINARY := "./build/gargantuan"

# Lists available recipes
default:
    just -l

# Builds the engine
build:
    cmake --build build -j={{ num_cpus() }}

# Rebuilds the engine, then runs an example inside assets
fresh_example example: build
    {{ GARGANTUAN_BINARY }} --script=./assets/examples/{{ example }}

# Rebuilds the engine, then runs the testbed project
fresh_testbed: build
    {{ GARGANTUAN_BINARY }} --project=./assets/testbed

# Runs an example inside assets without rebuilding
run_example example:
    {{ GARGANTUAN_BINARY }} --script=./assets/examples/{{ example }}

# Runs the testbed project without rebuilding
run_testbed:
    {{ GARGANTUAN_BINARY }} --project=./assets/testbed

# Runs tests for the core library
test_core: build
    lest run core

# Runs tests for all libraries
test: test_core

# Builds the documentation site, then begins watching for changes
docs:
    cd docs && astro dev
