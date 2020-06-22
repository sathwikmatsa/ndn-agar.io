# ndn-agar.io
An NDN port of Agar.io written in C++ using SDL2

![gameplay](https://user-images.githubusercontent.com/30603669/79047318-7cf79c80-7c33-11ea-9ca3-ef8395db7072.png)

# IP version
## Build
Ubuntu users can use this [setup script](https://github.com/sathwikmatsa/ndn-agar.io/blob/master/utils/install-deps-ubuntu.sh).
### dependecies
- [SDL2](https://lazyfoo.net/tutorials/SDL/01_hello_SDL/linux/index.php), [SDL2_image](https://lazyfoo.net/tutorials/SDL/06_extension_libraries_and_loading_other_image_formats/linux/index.php), [SDL2_ttf](https://lazyfoo.net/tutorials/SDL/16_true_type_fonts/index.php) (rendering)
- [sdplog](https://github.com/gabime/spdlog) (logging)
- [yojimbo](https://github.com/networkprotocol/yojimbo/blob/master/BUILDING.md), [libsodium](https://github.com/jedisct1/libsodium), [mbedtls](https://github.com/ARMmbed/mbedtls) (networking)


Note: Before you build yojimbo, run the script [prefix_macro_yojimbo.sh](https://github.com/sathwikmatsa/ndn-agar.io/blob/master/utils/prefix_macro_yojimbo.sh) in yojimbo directory. This is to remove macro collisions against the fmt library which is used by spdlog. Place the header files of yojimbo in [include/](https://github.com/sathwikmatsa/ndn-agar.io/tree/master/include/yojimbo) dir and libyojimbo.a (available in yojimbo's bin/ directory after build) in [lib/](https://github.com/sathwikmatsa/ndn-agar.io/tree/master/lib) directory. You can refere to ubuntu [setup script](https://github.com/sathwikmatsa/ndn-agar.io/blob/master/utils/install-deps-ubuntu.sh#L19-L52) for reference.

After setting up the above mentioned libraries, the Makefile should be able to compile the program.
```
> make all
```

## Run
After running make, two executables will be created namely ./bin/agario_client and ./bin/agario_server.

### Usage
```
Agario client

USAGE:
    agario_client <name> <server_address> [enable:Goblin]

ARGS:
    <name>              player name, max 7 characters
    <server_address>    ip address of server [ip:port]
    enable:Goblin       automates the gameplay (testing purposes)

FLAGS:
    -h, --help      Prints help information

Example:
    $> agario_client Dhruva 127.0.0.1:9999
```
```
Agario server

USAGE:
  agario_server <n>
 
ARGS:
  <n>     lobby capacity

Example:
   $> agario_server 3 # waits until 3 clients are connected and the game starts.
```

Note: when testing the build (non packaged version), run agario_client or agario_server from the root directory of the project. i.e,
```
$> pwd
xyz/downloads/ndn-agario/
$> ./bin/agario_client Dhruva 127.0.0.1:9999 enable:Goblin
```
this is to ensure the assets folder is accessible to the programs.
