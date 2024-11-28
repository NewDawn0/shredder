{
  description = "File deletion for the paranoid ones";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs";
    nix-systems.url = "github:nix-systems/default";
  };

  outputs = { self, nixpkgs, ... }@inputs:
    let eachSystem = nixpkgs.lib.genAttrs (import inputs.nix-systems);
    in {
      overlays.default =
        (final: prev: { note = self.packages.${prev.system}.default; });
      packages = eachSystem (system:
        let pkgs = nixpkgs.legacyPackages.${system};
        in {
          default = pkgs.buildGoModule {
            name = "shredder";
            src = ./.;
            vendorHash = null;
            meta = with pkgs.lib; {
              description = "Secure file deletion for the paranoid ones";
              homepage = "https://github.com/NewDawn0/shredder";
              maintainers = with maintainers; [ NewDawn0 ];
              license = licenses.mit;
            };
          };
        });
    };
}
