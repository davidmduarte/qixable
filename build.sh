set -e

exeName="qixable"
platform=$(uname)
srcFiles=$(echo src/*.c)

printf ".:: Building %s ::.\n\n" $exeName

if [ "$platform" = "Darwin" ]; then
    printf "\tPlatform: %s\n" "$platform"
    printf "\tCompiling ... \n"
    mkdir -p build
    gcc -o build/$exeName $srcFiles -O2 -lraylib -std=c99 -Wall
else
    printf "\n\tThe %s Platform, is not supported\n" "$platform"
    exit 1
fi

printf "\tCopying exe and assets to dist folder..\n"

mkdir -p dist
cp build/$exeName dist/
cp assets/*.png dist/
cp -R assets/categories/ dist/

printf "\tDone!\n"

printf "\n"
