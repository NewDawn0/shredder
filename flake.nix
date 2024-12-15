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
            pname = "shredder";
            version = "1.0.0";
            src = ./.;
            vendorHash = null;
            meta = {
              description =
                "A tool for securely deleting files to ensure privacy";
              longDescription = ''
                This tool securely deletes files, ensuring they cannot be recovered.
                Ideal for users who require extra privacy and need to ensure sensitive data is completely erased from their system.
              '';
              homepage = "https://github.com/NewDawn0/shredder";
              license = pkgs.lib.licenses.mit;
              maintainers = with pkgs.lib.maintainers; [ NewDawn0 ];
              platforms = pkgs.lib.platforms.all;
            };
          };
        });
    };
}
