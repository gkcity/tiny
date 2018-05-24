
# How to build

## step 0 download toolchains & ESP_IDF
http://esp-idf.readthedocs.io/en/latest/get-started/index.html#build-and-flash

## Step 1. Add tiny to ESP_IDF

0. add -DESP32 to CFLAGS in $ESP_IDF/make/project.mk

1. mkdir $ESP_IDF/components/tiny

2. mkdir $ESP_IDF/components/tiny/include

3. cp tiny/source/src/* $ESP_IDF/components/tiny/include

4. vi $ESP_IDF/components/tiny/compoment.mk


```
COMPONENT_ADD_INCLUDEDIRS := include/HttpClient         \
                             include/Json               \
                             include/Netty              \
                             include/StringArray        \
                             include/TinyBase           \
                             include/TinyBuffer         \
                             include/TinyContainer      \
                             include/TinyLog            \
                             include/TinyPortable       \
                             include/TinyRandom         \
                             include/TinyStringUtil

COMPONENT_SRCDIRS := library
```

## Step 2. Make


    ```
    make menuconfig
    make
    ```

## Step 3. copy libtiny.a


    ```
    cp tiny/target/libtiny_static.a build/tiny/libtiny.a
    ```

## Step 4. rebuild

    ```
    make
    ```

## Step 5. falsh

    ```
    make flash
    ```
