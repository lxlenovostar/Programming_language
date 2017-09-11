// Copyright © 2016 Alan A. A. Donovan & Brian W. Kernighan.
// License: https://creativecommons.org/licenses/by-nc-sa/4.0/

// See page 244.

// Countdown implements the countdown for a rocket launch.
package main

import (
	"fmt"
	"time"
)

//!+
func main() {
	fmt.Println("Commencing countdown.")
	// func Tick(d Duration) <-chan Time
	//
	// Tick is a convenience wrapper for NewTicker providing
	// access to the ticking channel only. While Tick is useful
	// for clients that have no need to shut down the Ticker,
	// be aware that without a way to shut it down the underlying
	// Ticker cannot be recovered by the garbage collector;
	// it "leaks". Unlike NewTicker, Tick will return nil if d <= 0.
	tick := time.Tick(1 * time.Second)
	for countdown := 10; countdown > 0; countdown-- {
		fmt.Println(countdown)
		<-tick
	}
	launch()
}

//!-

func launch() {
	fmt.Println("Lift off!")
}
