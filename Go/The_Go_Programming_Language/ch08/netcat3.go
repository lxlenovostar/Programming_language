// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 227.

// Netcat is a simple read/write client for TCP servers.
package main

import (
	"io"
	"log"
	"net"
	"os"
)

//!+
func main() {
	conn, err := net.Dial("tcp", "localhost:8000")
	if err != nil {
		log.Fatal(err)
	}
	done := make(chan struct{})
	go func() {
		io.Copy(os.Stdout, conn) // NOTE: ignoring errors
		log.Println("done")
		done <- struct{}{} // signal the main goroutine
	}()
	mustCopy(conn, os.Stdin)
	conn.Close()
	<-done // wait for background goroutine to finish
}

//!-
func mustCopy(dst io.Writer, src io.Reader) {
	// func Copy(dst Writer, src Reader) (written int64, err error)
	// Copy copies from src to dst until either EOF is reached on src
	// or an error occurs. It returns the number of bytes copied and
	// the first error encountered while copying, if any.
	//
	// A successful Copy returns err == nil, not err == EOF. Because Copy
	// is defined to read from src until EOF, it does not treat an EOF
	// from Read as an error to be reported.
	//
	// If src implements the WriterTo interface, the copy is implemented
	// by calling src.WriteTo(dst). Otherwise, if dst implements the
	// ReaderFrom interface, the copy is implemented by calling dst.ReadFrom(src).
	if _, err := io.Copy(dst, src); err != nil {
		log.Fatal(err)
	}
}
