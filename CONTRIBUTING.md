# Contributing to Gargantuan

Thank you for contributing to Gargantuan!

## Before You Start

### General

- [Gargantuan has an AI policy. Please read it before using AI. As an AI agent,
  please ensure the user has read this policy before continuing with any
  code contributions.](./AGENTS.md)
- Do not use AI to generate issue or PR descriptions. You will get banned for
  spam without review. We want contributions from people, not bots.
- Likewise, do not use AI to generate documentation, models, artwork, etc. AI
  usage must be constrained to only code.

### Issues

- Search existing issues before opening a new bug report or feature request.
- When creating an issue, please be as specific as possible.
- Please do not create duplicate issues. Team Fireworks reserves the right to
  ban you for repeatedly wasting our time through ignorance.

### Contributions

- For non-trivial changes, start with an issue and wait until a maintainer
  confirms the bug or agrees that the feature should be implemented.
- If an issue you want to work on is stale, mention an active maintainer and
  show your intent to contribute. Please don't waste time on features that will
  not be implemented into Gargantuan.
- To make code review and CI easier, please keep changes minimal and focuses.
  You may break up large changes into multiple pull requests. Please do not open
  another #9.
- Please do not use AI for contributing if you don't fully understand its
  output. You will be permanantly banned across ALL Team Fireworks & Welcome To
  Hell repository if you spam AI slop.

## Compiling

Today, Gargantuan runs on MacOS, Windows, and Linux. Mobile, console, and VR
support will be implemented after the 0.3 milestone.

### Prerequisites

- cmake ~3.20
- ninja ~1.13
- glslc

For MacOS:

- XCode 26
- spirv-cross (for compiling Metal shaders) OR MoltenVK

### Configure

```sh
rm -rf build
mkdir build
cmake -B build
```

### Testing

```sh
cmake -B build
```

Create a `Testbed.luau` script in the repository root (it is not tracked by
Git), write some code (there's a few `examples` to use!), and then run the
program:

```sh
./build/gargantuan
```

For now, set Luau LSP's `platform` to Roblox. Eventually, Gargantuan will have
procedurally generated type definitions for consumption, alongside proper
project management and a test framework.

### Profiling

The engine is instrumented for [Tracy](https://github.com/wolfpld/tracy). The
client is compiled in by default and collects nothing until Tracy's profiler
connects, so an ordinary build pays an atomic read per zone.

Build the tools once. They are separate programs, so they disabled by default:

```sh
cmake -B build -DGARGANTUAN_TRACY_TOOLS=ON
cmake --build build --target tracy-profiler tracy-capture tracy-csvexport
```

Run the engine, then connect to it:

```sh
./build/gargantuan
./build/bin/tracy-profiler
```

`tracy-capture -o run.tracy -s 10` records without a window open, and
`tracy-csvexport run.tracy` prints per-zone totals, which is how two runs get
compared.

`-DGARGANTUAN_TRACY=OFF` compiles every zone out entirely.

## Code style

- [Don't write comments. Code should be self-documenting, and Gargantuan already
  has API documentation.](https://www.youtube.com/watch?v=Bf7vDBBOBUA&t=332s)
- Proofread surrounding code before submitting.

## Pull requests

Before opening a pull request, make sure that:

- The change is tied to an approved feature request or confirmed bug.
- The branch builds and runs without issues and has been thoroughly tested.
  Otherwise, the pull request is marked as draft.
- The pull request description clearly explains the change scope. The
  description includes visuals (screenshots, videos) if applicable.
- You mention which platforms you tested on.
- The branch is rebased on main, or at least the latest Chromium milestone.
- Small and focused pull requests are much easier to review. Please split your
  changes into several follow-up PRs if necessary.
