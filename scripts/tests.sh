#!/bin/bash

for i in {1..1000}; do
	./bin/release/main <<< "$i"
done
