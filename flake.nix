{
  description = "Super flake";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-24.05";
    flake-utils.url = "github:numtide/flake-utils";
  };
  
  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem
      (system:
        let pkgs = nixpkgs.legacyPackages.${system}; in
        {
          devShells.default = import ./shell.nix { inherit pkgs; };
        }
      );

  # outputs = { self, nixpkgs }: {
  #   devShells.default = nixpkgs.mkShell {
  #       buildInputs = with nixpkgs; [
  #         mingw_w64_gcc
  #         mingw_w64_binutils
  #         mingw_w64_crt
  #         mingw_w64_headers
  #         mingw_w64_winpthreads
  #         (python3.withPackages (ps: [ ps.flask ]))
  #         nodejs
  #       ];
  #     };
  #   };
  # };

#   packages.x86_64-linux.super = pkgs: pkgs.stdenv.mkDerivation {
#     pname = "super";
#     version = "1.0.0";
#     src = ./.;
#
#     buildInputs = [
#       pkgs.mingw_w64_gcc
#       pkgs.mingw_w64_binutils
#       pkgs.mingw_w64_crt
#       pkgs.mingw_w64_headers
#       pkgs.mingw_w64_winpthreads
#     ];
#
#     nativeBuildInputs = [ pkgs.autoconf pkgs.automake pkgs.libtool ];
#
#     configurePhase = ''
#       mkdir -p build
#     '';
#
#     buildPhase = ''
#       x86_64-w64-mingw32-gcc ${./hello.c} -o build/hello.exe
#     '';
#
#     installPhase = ''
#       mkdir -p $out/bin
#       cp build/super.exe $out/bin/
#     '';
#   };
}
