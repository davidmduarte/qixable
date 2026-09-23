set -e

exeName="qixable"
platform=$(uname)
srcFiles=$(echo src/*.c)

printf ".:: Building %s ::.\n\n" $exeName

if [ "$platform" = "Darwin" ]; then
    printf "\tPlatform: %s\n" "$platform"
    printf "\tCompiling ... \n"
    mkdir -p build
    tcc -o build/$exeName $srcFiles -O2 -lraylib -std=c99 -Wall

elif [ "$platform" = "Linux" ]; then
    printf "\tPlatform: %s\n" "$platform"
    printf "\tCompiling ... \n"
    mkdir -p build
    tcc -o build/$exeName $srcFiles -O2 -std=c99 -Wall -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
		 
else
    printf "\n\tThe %s Platform, is not supported\n" "$platform"
    exit 1
fi

printf "\tCopying exe and assets to dist folder..\n"

mkdir -p dist
cp build/$exeName dist/
cp -R assets/* dist/

printf "\tDone!\n"

sourceLines=$(cat src/*.c src/*.h | wc -l)
printf "\tSource lines: %s\n" "$sourceLines"

printf "\n"
