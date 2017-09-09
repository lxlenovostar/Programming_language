// clockwall listens to multiple clock servers concurrently.
package main

import (
	"bufio"
	"fmt"
	"io"
	"log"
	"net"
	"os"
	"strings"
	"time"
)

type clock struct {
	name, host string
}

func (c *clock) watch(w io.Writer, r io.Reader) {
	// Package bufio implements buffered I/O. It wraps
	// an io.Reader or io.Writer object, creating another
	// object (Reader or Writer) that also implements the
	// interface but provides buffering and some help for textual I/O.
	//
	// NewScanner returns a new Scanner to read from r.
	// The split function defaults to ScanLines.
	s := bufio.NewScanner(r)
	// Scan advances the Scanner to the next token, which
	// will then be available through the Bytes or Text method.
	// It returns false when the scan stops, either by reaching
	// the end of the input or an error. After Scan returns false,
	// the Err method will return any error that occurred during
	// scanning, except that if it was io.EOF, Err will return nil.
	// Scan panics if the split function returns 100 empty tokens
	// without advancing the input. This is a common error mode for scanners.
	for s.Scan() {
		// Text returns the most recent token generated by a
		// call to Scan as a newly allocated string holding its bytes.
		fmt.Fprintf(w, "%s: %s\n", c.name, s.Text())
	}
	fmt.Println(c.name, "done")
	if s.Err() != nil {
		log.Printf("can't read from %s: %s", c.name, s.Err())
	}
}

func main() {
	if len(os.Args) == 1 {
		fmt.Fprintln(os.Stderr, "usage: clockwall NAME=HOST ...")
		os.Exit(1)
	}
	clocks := make([]*clock, 0)
	for _, a := range os.Args[1:] {
		fields := strings.Split(a, "=")
		if len(fields) != 2 {
			fmt.Fprintf(os.Stderr, "bad arg: %s\n", a)
			os.Exit(1)
		}
		clocks = append(clocks, &clock{fields[0], fields[1]})
	}
	for _, c := range clocks {
		conn, err := net.Dial("tcp", c.host)
		if err != nil {
			log.Fatal(err)
		}
		defer conn.Close()
		go c.watch(os.Stdout, conn)
	}
	// Sleep while other goroutines do the work.
	for {
		time.Sleep(time.Minute)
	}
}
