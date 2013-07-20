#!/bin/sh
# Run all unit tests

TMP_OUTPUT=tmp-output.txt

run_command() {
	"$@" 2>&1 1> $TMP_OUTPUT
	if [ $? -ne 0 ]; then
		cat $TMP_OUTPUT
		rm -f $TMP_OUTPUT
		echo "$1 returned exit status $?"
		echo "output has been written to $TMP_OUTPUT"
		exit 1
	fi
	rm -f $TMP_OUTPUT
}

run_test() {
	DIR="$1"
	cd "$DIR"
	if [ -f "test${DIR}.pro" ]; then
		echo "running test: ${DIR}"
		run_command qmake
		run_command make
		run_command ./test${DIR}
	else
		echo "warning: ${DIR}/${DIR}.pro not found"
	fi
	cd ..
}

for dir in *; do
	if [ -d "$dir" ]; then # it is a directory
		run_test "$dir"
	fi
done
