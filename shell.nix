{ pkgs ? import (fetchTarball "https://github.com/NixOS/nixpkgs/tarball/nixos-23.11") {} }:

pkgs.mkShellNoCC {
  packages = with pkgs; [
    (python3.withPackages (ps: [ ps.flask ps.flask-login ps.flask-wtf ps.flask-cors ps.psycopg2 ps.sqlalchemy ps.python-dotenv ps.requests ps.beautifulsoup4]))
    nodejs
    postgresql
  ];
}
