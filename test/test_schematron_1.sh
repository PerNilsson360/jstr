#! /bin/bash
cat <<< '{"a":3}' | ../src/jstr --schema=schematron-1.json
