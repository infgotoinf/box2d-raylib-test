{pkgs ? import <nixpkgs> {}}:
pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    raylib
    box2d

    libGL

    # X11 dependencies
    libX11
    libX11.dev
    libXcursor
    libXi
    libXinerama
    libXrandr

    # Web support (uncomment to enable)
    # pkgs.emscripten

    cmake
    ninja
    ccache

    clang-tools
  ];

  CMAKE_CXX_COMPILER_LAUNCHER = "ccache";

  # Audio dependencies
  LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath [pkgs.alsa-lib];
}
