#! /bin/bash
function execute_test() {
    cat <<< '{"a":3}' | ../src/jstr --schema=schematron-2.json
    RETURN_CODE=$?
    if [ $RETURN_CODE -eq 255 ]; then
        return 0
    else
        return 255
    fi
}
execute_test
