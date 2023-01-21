#!/bin/bash

GREEN='\033[32m'
RED='\033[31m'
NC='\033[0m'

# Test 1: Program prints error message in response to
# invocation error

INTERSECT=./intersect

# Define the sample files to use as input
FILES=""

# Define the options to pass to the program
OPTIONS=""

#Define the expected output
EXPECTED_OUTPUT="Usage: ./intersect FILE1 FILE2 \[FILE 3...\]"

# Run the program with the sample files and options
$INTERSECT $OPTIONS ${FILES[@]} 2> output.txt

# Expected: Program exits with code 1 for INVOCATION_ERROR
# and outputs usage statement
if [ $? -eq 1 ] && grep -q "$EXPECTED_OUTPUT" output.txt; then
    echo -e "1. Invocation Error test              : ${GREEN}PASS${NC}"
else
    echo -e "1. Invocation Error test              : ${RED}FAIL${NC}"
fi

# Test 2: program does not crash with empty file

INTERSECT=./intersect
FILES="./samp/empty.txt ./samp/empty.txt"
OPTIONS=""

$INTERSECT $OPTIONS ${FILES[@]}

# Expected: Program exits with code 0 for SUCCESS
if [ $? -eq 0 ]; then
    echo -e "2. Empty file test                    : ${GREEN}PASS${NC}"
else
    echo -e "2. Empty file test                    : ${RED}FAIL${NC}"
fi

# Test 3: Program sorts alphabetically with no options

INTERSECT=./intersect
FILES="./samp/phonetic.txt ./samp/underscore.txt"
OPTIONS=""
echo "Alpha
_Alpha
Bravo
Bravo_" > cmp.txt

$INTERSECT $OPTIONS ${FILES[@]} > output.txt

# Expected: Program exits with code 0 for SUCCESS and prints
# above expected output
if [ $? -eq 0 ] && cmp -s cmp.txt output.txt; then
    echo -e "3. Default sort test                  : ${GREEN}PASS${NC}"
else
    echo -e "3. Default sort test                  : ${RED}FAIL${NC}"
fi

# Test 4: Program prints alternate casings with -a option

INTERSECT=./intersect
FILES="./samp/phonetic.txt ./samp/underscore.txt"
OPTIONS="-a"
echo "Alpha ALPHA _Alpha
Bravo BRAVO Bravo_" > cmp.txt

$INTERSECT $OPTIONS ${FILES[@]} > output.txt

# Expected: Program exits with code 0 for SUCCESS and prints
# above expected output
if [ $? -eq 0 ] && cmp -s cmp.txt output.txt; then
    echo -e "4. Alternate casing print test        : ${GREEN}PASS${NC}"
else
    echo -e "4. Alternate casing print test        : ${RED}FAIL${NC}"
fi

# Test 5: Program prints ignoring leading/trailing punctuation 
# with -i option

INTERSECT=./intersect
FILES="./samp/phonetic.txt ./samp/underscore.txt"
OPTIONS="-i"
echo "Alpha
Bravo" > cmp.txt

$INTERSECT $OPTIONS ${FILES[@]} > output.txt

# Expected: Program exits with code 0 for SUCCESS and prints
# above expected output
if [ $? -eq 0 ] && cmp -s cmp.txt output.txt; then
    echo -e "5. Punctuation-insensitive print test : ${GREEN}PASS${NC}"
else
    echo -e "5. Punctuation-insensitive print test : ${RED}FAIL${NC}"
fi

# Test 6: Program prints duplicate casings while
# ignoring leading/trailing punctuation with -ai options

INTERSECT=./intersect
FILES="./samp/phonetic.txt ./samp/underscore.txt"
OPTIONS="-ai"
echo "Alpha ALPHA
Bravo BRAVO" > cmp.txt

$INTERSECT $OPTIONS ${FILES[@]} > output.txt

# Expected: Program exits with code 0 for SUCCESS and prints
# above expected output
if [ $? -eq 0 ] && cmp -s cmp.txt output.txt; then
    echo -e "6. All-options print test             : ${GREEN}PASS${NC}"
else
    echo -e "6. All-options print test             : ${RED}FAIL${NC}"
fi

# Test 7: Program succesfully reads when multiple words
# are on the same line

INTERSECT=./intersect
FILES="./samp/phonetic.txt ./samp/sameline.txt"
OPTIONS=""
echo "Alpha
Bravo
Charlie
Delta" > cmp.txt

$INTERSECT $OPTIONS ${FILES[@]} > output.txt

# Expected: Program exits with code 0 for SUCCESS and prints
# above expected output
if [ $? -eq 0 ] && cmp -s cmp.txt output.txt; then
    echo -e "7. Multi-word line test               : ${GREEN}PASS${NC}"
else
    echo -e "7. Multi-word line test               : ${RED}FAIL${NC}"
fi

# Test 8: Program successfully processes two large files

INTERSECT=./intersect
FILES="./samp/american-english.txt ./samp/words.txt"

$INTERSECT $OPTIONS ${FILES[@]} >/dev/null

# Expected: Program exits with code 0 for SUCCESS and 
# does not crash, take excessive time or enter infinite loop
if [ $? -eq 0 ]; then
    echo -e "8. Two Very large files test          : ${GREEN}PASS${NC}"
else
    echo -e "8. Two Very large files test          : ${RED}FAIL${NC}"
fi

# Test 9: Program succesfully processes several large files

INTERSECT=./intersect
FILES="./samp/american-english.txt ./samp/words.txt ./samp/words.txt ./samp/american-english.txt"

$INTERSECT $OPTIONS ${FILES[@]} >/dev/null

# Expected: Program exits with code 0 for SUCCESS and 
# does not crash, take excessive time or enter infinite loop
if [ $? -eq 0 ]; then
    echo -e "9. Four very large files test         : ${GREEN}PASS${NC}"
else
    echo -e "9. Four very large files test         : ${RED}FAIL${NC}"
fi

# Cleanup temp files
rm cmp.txt output.txt

