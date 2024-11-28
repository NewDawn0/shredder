# Shredder : Secure file deletion for the paranoid

## Overview

The **Shredder** script securely deletes files and directories by overwriting them with random data and zeroes, ensuring that the data cannot be recovered. It is designed to be run with a single command, `shredder <path/to/file_or_directory>`, to securely erase sensitive files from your system.

### Usage

To use the Shredder script, simply run it from the command line as follows:

```bash
shredder <path/to/file_or_directory>
```

This will securely delete the specified file or directory. It is recommended to run the script with **root** privileges to ensure it has access to all files and directories you want to shred. You can run it as root using:

```bash
sudo shredder <path/to/file_or_directory>
```

### How It Works

The Shredder script performs a multi-step process to securely delete files:

1. **Overwrite with Random Data**: It first overwrites the contents of the file with random data sourced from `/dev/urandom`, making it difficult to recover any original data.
2. **Overwrite with Zeroes**: After the random data is written, it overwrites the file again with zeroes sourced from `/dev/zero` to ensure any remaining information is wiped.

3. **File Deletion**: Finally, the file or directory is deleted from the filesystem, making recovery almost impossible.

This ensures that sensitive data is irrecoverably destroyed by overwriting it differently a multitude of times, making it nearly impossible to recover files deleted this way.

### Installation

Shredder is installable using either Go or Nix.

#### Installation via Go

1. Clone the repository:

   ```bash
   git clone https://github.com/NewDawn0/shredder.git
   cd shredder
   ```

2. Build the script with Go:

   ```bash
   go build -o shredder ./src/cmd/shredder/main.go
   ```

3. You can now use the script with the `shredder` command.

#### Installation via Nix (with Flake support)

If you're using Nix, you can install Shredder directly from the provided `flake.nix` file.

1. Make sure you have Nix installed and flakes enabled.

2. Run the following command to install Shredder:

   ```bash
   nix run github:NewDawn0/shredder
   ```

3. You can also build it locally using:

   ```bash
   nix build github:NewDawn0/shredder
   ```

Once installed, you can run the script with the `shredder` command as described in the usage section.
