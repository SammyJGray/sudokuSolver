#!/bin/bash

for i in {1..1000}; do
	./bin/main <<< "$i"
done
