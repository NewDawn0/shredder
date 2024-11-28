package main

import (
	"fmt"
	"io"
	"os"
	"path/filepath"
)

const randomFile = "/dev/urandom"
const zeroFile = "/dev/zero"
const red = "\x1b[31;1m"
const green = "\x1b[32;1m"
const nc = "\x1b[0m"

func main() {
	if len(os.Args) != 2 {
		fmt.Printf("%sError:%s provide a single path\n", red, nc)
		os.Exit(1)
	} else {
		files, insufficientPerms, err := getFiles(os.Args[1])
		if len(insufficientPerms) > 0 {
			fmt.Printf("%sError:%s Insufficient write permissions for: %s[%s\n %v %s]%s\n", red, nc, red, nc, insufficientPerms, red, nc)
			os.Exit(1)
		}
		if err != nil {
			fmt.Printf("%sError:%s %v\n", red, nc, err)
			return
		}
		for count, file := range files {
			err = shredFile(file, count+1, len(files))
			if err != nil {
				fmt.Printf("%sError:%s %v\n", red, nc, err)
			}
		}
		fmt.Printf("%sInfo:%s Deleting %s\n", green, nc, os.Args[1])
		os.RemoveAll(os.Args[1])
		return
	}
}

func getFiles(path string) ([]string, []string, error) {
	changedFiles := []string{}
	insufficientPerms := []string{}
	err := filepath.Walk(path, func(path string, info os.FileInfo, err error) error {
		if err != nil {
			return err
		}
		if info.IsDir() {
			return nil
		}
		// Skip symlinks
		lstatInfo, err := os.Lstat(path)
		if err != nil {
			return err
		}
		if lstatInfo.Mode()&os.ModeSymlink != 0 {
			fmt.Printf("%sInfo:%s Deleting symlink: %s\n", green, nc, path)
			os.Remove(path)
			return nil
		}
		if info.Mode().Perm()&os.ModePerm == os.ModePerm { // Check for write permission
			insufficientPerms = append(insufficientPerms, path)
			return nil
		}
		changedFiles = append(changedFiles, path)
		fmt.Printf("\x1b[2KSearching files: [%d]\r", len(changedFiles))
		return nil
	})
	if err != nil {
		return nil, nil, err
	}
	return changedFiles, insufficientPerms, nil
}

func shredFile(path string, count, total int) error {
	// File
	fmt.Printf("\x1b[2KOpening files: [%d/%d]\r", count, total)
	// Open in read-write mode
	file, err := os.OpenFile(path, os.O_RDWR, 0600)
	if err != nil {
		return fmt.Errorf("Could not open %s: %v", err, path)
	}
	defer file.Close()

	// Random and zero
	random, err := os.Open(randomFile)
	if err != nil {
		return fmt.Errorf("Could not open %s: %v", err, randomFile)
	}
	defer random.Close()

	zero, err := os.Open(zeroFile)
	if err != nil {
		return fmt.Errorf("Could not open %s: %v", err, zeroFile)
	}
	defer zero.Close()

	stat, err := file.Stat()
	if err != nil {
		return fmt.Errorf("Could not get file stats: %v", err)
	}
	size := stat.Size()

	// Seek beginning of file
	_, err = file.Seek(0, io.SeekStart)
	if err != nil {
		return fmt.Errorf("Unable to seek in file: %v", err)
	}

	// Overwrite with random data
	fmt.Printf("\x1b[2KMaking file random: [%d/%d]\r", count, total)
	_, err = io.CopyN(file, random, size)
	if err != nil {
		return fmt.Errorf("Unable to overwrite %s with data from %s: %v", path, randomFile, err)
	}
	// Seek beginning of file
	_, err = file.Seek(0, io.SeekStart)
	if err != nil {
		return fmt.Errorf("Unable to seek in file: %v", err)
	}

	// Zero file
	fmt.Printf("\x1b[2KZeroing file: [%d/%d]\r", count, total)
	_, err = io.CopyN(file, zero, size)
	if err != nil {
		return fmt.Errorf("Unable to overwrite %s with data from %s: %v", path, zeroFile, err)
	}
	os.Remove(path)
	return nil
}
