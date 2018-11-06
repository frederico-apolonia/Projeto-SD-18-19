#!/bin/bash

create_missing_dirs() {
    if [ ! -d "binary" ]; then
        mkdir binary
    fi

    if [ ! -d "object" ]; then
        mkdir object
    fi
}

clean_and_compile() {
    create_missing_dirs
    make clean; make
}

create_all_tests() {
    clean_and_compile
    make test_data
    make test_entry
    make test_list
    make test_table
}

# runs tests with valgrind
run_all_tests_valgrind() {
    create_all_tests
    echo "--- RUNNING TEST_DATA ---"
    valgrind ./binary/test_data
    echo "--- RUNNING TEST_ENTRY ---"
    valgrind ./binary/test_entry
    echo "--- RUNNING TEST_LIST ---"
    valgrind ./binary/test_list
    echo "--- RUNNING TEST_TABLE ---"
    valgrind ./binary/test_table
}

run_test_data() {
    clean_and_compile
    echo "--- COMPILING TEST_DATA ---"
    make test_data
    echo "--- RUNNING TEST_DATA ---"
    valgrind ./binary/test_data
}

run_test_entry() {
    clean_and_compile
    echo "--- COMPILING TEST_ENTRY ---"
    make test_entry
    echo "--- RUNNING TEST_ENTRY ---"
    valgrind ./binary/test_entry
}

run_test_list() {
    clean_and_compile
    echo "--- COMPILING TEST_LIST ---"
    make test_list
    echo "--- RUNNING TEST_LIST ---"
    valgrind ./binary/test_list
}

run_test_table() {
    clean_and_compile
    echo "--- COMPILING TEST_TABLE ---"
    make test_table
    echo "--- RUNNING TEST_TABLE ---"
    valgrind ./binary/test_table
}

