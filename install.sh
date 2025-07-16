#!/bin/bash
JSON_C=0
INSTALL=1
CLEAN=0
HELP=0

while [[ $# -gt 0 ]]; do
    key="$1"

    case $key in
        --json-c)
            JSON_C=1

            shift
            ;;

        --no-install)
            INSTALL=0

            shift
            ;;

        --clean)
            CLEAN=1

            shift
            ;;

        --help)
            HELP=1

            shift
            ;;
        *)

            shift
            ;;
        esac
done

if [ "$HELP" -gt 0 ]; then
    echo "Usage: install.sh [OPTIONS]"
    echo
    echo "Options:"
    echo "  --json-c       Build and install JSON-C before building the tool."
    echo "  --no-install   Build the web server and/or JSON-C without installing them."
    echo "  --clean        Remove build files for the web server and JSON-C."
    echo "  --help         Display this help message."

    exit 0
fi

if [ "$CLEAN" -gt 0 ]; then
    if [ "$JSON_C" -gt 0 ]; then
        echo "Cleaning JSON-C..."

        ./scripts/json-c_clean.sh
    fi

    echo "Cleaning web server..."

    ./scripts/clean.sh

    exit 0
fi

if [ "$JSON_C" -gt 0 ]; then
    echo "Building JSON-C..."

    ./scripts/json-c_build.sh

    if [ "$INSTALL" -gt 0 ]; then
        echo "Installing JSON-C..."

        sudo ./scripts/json-c_install.sh
    fi
fi

echo "Building web server..."

./scripts/build.sh

if [ "$INSTALL" -gt 0 ]; then
    echo "Installing web server..."

    sudo ./scripts/install.sh
fi