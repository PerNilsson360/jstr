#! /bin/bash
function execute_test() {
    echo '{"ipv4":[{"addr":"1.2.3.4","port":22},{"addr":"1.2.3.4","port":23}, {"addr":"1.2.3.4","port":24}]}' | ../src/jstr --schema=schematron-3.json
    if [ $? -ne 0 ]; then
        return 255
    fi
    echo '{"ipv4":[{"addr":"1.2.3.4","port":22},{"addr":"1.2.3.4","port":23}, {"addr":"1.2.3.4","port":22}]}' | ../src/jstr --schema=schematron-3.json
    if [ $? -ne 255 ]; then
        return 255
    fi
    echo '{"ipv4":[{"addr":"1.2.3.4","port":22},{"port":23,"addr":"1.2.3.4"}]}' | ../src/jstr --schema=schematron-3.json
    if [ $? -ne 0 ]; then
        return 255
    fi
    echo '{"ipv4":[{"addr":"1.2.3.4","port":22},{"port":22,"addr":"1.2.3.4"}]}' | ../src/jstr --schema=schematron-3.json
    if [ $? -ne 255 ]; then
        return 255
    fi

}
execute_test
