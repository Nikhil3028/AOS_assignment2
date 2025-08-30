#!/bin/bash

echo "=== Testing Pipe Functionality ==="
echo

# Test 1: Internal command to external command
echo "Test 1: echo hello | grep hello"
echo "echo hello | grep hello" | timeout 5 ./shell
echo

# Test 2: Internal command to internal command  
echo "Test 2: echo test | cat"
echo "echo test | cat" | timeout 5 ./shell
echo

# Test 3: External command to external command
echo "Test 3: cat /etc/passwd | head -3"
echo "cat /etc/passwd | head -3" | timeout 5 ./shell
echo

# Test 4: Multiple pipes
echo "Test 4: echo hello world | cat | wc -w"
echo "echo hello world | cat | wc -w" | timeout 5 ./shell
echo

# Test 5: ls with pipe
echo "Test 5: ls | head -5"
echo "ls | head -5" | timeout 5 ./shell
echo

echo "=== Tests Complete ==="
