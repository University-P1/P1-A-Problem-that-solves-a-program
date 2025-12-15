{
  description = "A wildfire simulation tool for a university first semester project";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs =
    { nixpkgs, ... }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
      lib = nixpkgs.lib;
    in
    {
      formatter.${system} = pkgs.nixfmt-rfc-style;

      devShells.${system}.default = pkgs.mkShell.override { inherit (pkgs.llvmPackages_latest) stdenv; } rec {
        packages = with pkgs; [
          zig
          cmake
          ninja
          llvmPackages_latest.lld
          (python3.withPackages (p: [
            p.customtkinter
            p.tkinter
          ]))
          llvmPackages_latest.lldb

          llvmPackages_latest.llvm

          wayland
          wayland-scanner
          wayland-protocols

          libxkbcommon
          egl-wayland
          pkg-config
          libGL
          vulkan-headers
          vulkan-loader
          libdecor

          (lib.getDev sdl3)
          (lib.getDev sdl3-ttf)
          (lib.getDev sdl3-image)
        ];

        NIX_LDFLAGS =
          "-rpath ${
            lib.makeLibraryPath packages
          }";
        };
    };
}
