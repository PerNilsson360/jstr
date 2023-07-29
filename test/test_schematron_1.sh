#! /bin/bash
function execute_test() {
    echo '{"a":3}' | ../src/jstr --schema=schematron-1.json
    if [ $? -ne 0 ]; then
        return 255
    fi
    echo '{"a":[1, 2, 3]}' | ../src/jstr --schema=schematron-1.json
    if [ $? -ne 255 ]; then
        return 255
    fi
    return 0
}
execute_test
