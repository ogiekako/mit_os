package main

import (
	"flag"
	"net/http"
)

var port = flag.String("port", "8080", "Define the port to bind to")
var root = flag.String("root", ".", "Define the root filesystem path")

func main() {
	flag.Parse()
	panic(http.ListenAndServe(":"+*port, http.FileServer(http.Dir(*root))))
}
