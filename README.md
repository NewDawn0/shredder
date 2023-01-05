# shredder
shredder - A File/Folder shredding utility

<!-- vim-markdown-toc GFM -->

* [Installation](#installation)
* [Usage](#usage)
* [Future plans](#future-plans)

<!-- vim-markdown-toc -->

### Installation
```bash
git clone https://github.com/NewDawn0/shredder.git
cd shredder/src
gcc -o shredder shredder.c

# optional
sudo mv shredder /usr/local/bin
```

### Usage
```bash
shredder - Fast threaded secure file & folder deletion
shredder <files/options>
	-h		 Shows this menu
	--help		 Shows this menu
	-d		 Prints debug log
	--debug		 Prints debug log
	--no-confirm	 Skips the deletion confirmation
```

### Future plans
- [x] Add makefile
- [ ] Use C only without system() and sh
