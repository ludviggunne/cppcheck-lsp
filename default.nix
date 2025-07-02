{
  pkgs ? import <nixpkgs> {},
}:

pkgs.stdenv.mkDerivation {
  name = "cppcheck-lsp";
  phases = [ "unpackPhase" "buildPhase" "installPhase" ];
  installPhase = ''
    make install PREFIX=$out
  '';
}
