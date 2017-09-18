package main

import (
	"bufio"
	"fmt"
	"strings"
)

type WordCounter int

func (wc *WordCounter) Write(p []byte) (int, error) {
	// Package bufio implements buffered I/O. It wraps an
	// io.Reader or io.Writer object, creating another object
	// (Reader or Writer) that also implements the interface
	// but provides buffering and some help for textual I/O.

	// func NewScanner(r io.Reader) *Scanner
	// NewScanner returns a new Scanner to read from r. The split
	// function defaults to ScanLines.

	// A Reader implements the io.Reader, io.ReaderAt, io.Seeker,
	// io.WriterTo, io.ByteScanner, and io.RuneScanner interfaces by
	// reading from a string.
	// type Reader struct {
	//        // contains filtered or unexported fields
	//}

	// func NewReader(s string) *Reader
	// NewReader returns a new Reader reading from s. It is similar
	// to bytes.NewBufferString but more efficient and read-only.
	scanner := bufio.NewScanner(strings.NewReader(string(p)))

	// func ScanWords(data []byte, atEOF bool) (advance int, token []byte, err error)
	// ScanWords is a split function for a Scanner that returns
	// each space-separated word of text, with surrounding spaces
	// deleted. It will never return an empty string. The definition
	// of space is set by unicode.IsSpace.

	// func (s *Scanner) Split(split SplitFunc)
	// Split sets the split function for the Scanner. The default split
	// function is ScanLines. Split panics if it is called after scanning
	// has started.

	// Set the split function for the scanning operation.
	scanner.Split(bufio.ScanWords)

	c := 0
	// func (s *Scanner) Scan() bool
	// Scan advances the Scanner to the next token,
	// which will then be available through the Bytes or Text method.
	// It returns false when the scan stops, either by reaching the
	// end of the input or an error. After Scan returns false, the Err
	// method will return any error that occurred during scanning,
	// except that if it was io.EOF, Err will return nil. Scan panics
	// if the split function returns 100 empty tokens without advancing
	// the input. This is a common error mode for scanners.
	for scanner.Scan() {
		c++
	}
	*wc += WordCounter(c)
	return c, nil
}

type LineCounter int

func (lc *LineCounter) Write(p []byte) (int, error) {
	c := 1
	for _, b := range p {
		if b == '\n' {
			c++
		}
	}
	*lc += LineCounter(c)
	return c, nil
}

func main() {
	var wc WordCounter
	wc.Write([]byte("Hello word! 你好"))
	fmt.Println(wc)
	var lc LineCounter
	lc.Write([]byte(`hello
abc
def ad fas 
`))
	fmt.Println(lc)
}
