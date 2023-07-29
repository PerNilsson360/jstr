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
    # TODO here it should be possible to switch places on addr and port
    return 0                    # success
}
execute_test
