{
  description = "Fast file deletion for the paranoid ones";

  inputs.utils.url = "github:NewDawn0/nixUtils";

  outputs = { self, utils, ... }: {
    overlays.default = final: prev: {
      shredder = self.packages.${prev.system}.default;
    };
    packages = utils.lib.eachSystem { } (pkgs: {
      default = pkgs.buildGoModule {
        pname = "shredder";
        version = "1.0.0";
        src = ./.;
        vendorHash = null;
        meta = {
          description = "File deletion for the paranoid ones";
          longDescription = ''
            "A tool for securely deleting files to ensure privacy";
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
