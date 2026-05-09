# hwlt_pico2

Repozytorium projektu Raspberry Pi Pico oparte na przykładzie `blink_simple`.

## Zawartość

- `blink_simple.c` — program migającej diody LED dla Raspberry Pi Pico
- `CMakeLists.txt` — konfiguracja budowania projektu
- `pico-sdk/` — submoduł Raspberry Pi Pico SDK
- `pico-examples/` — submoduł oficjalnych przykładów Raspberry Pi Pico

## Instalacja

1. Sklonuj repozytorium razem z submodułami:

   ```bash
git clone --recurse-submodules <repo-url>
```

2. Lub po sklonowaniu uruchom:

   ```bash
git submodule update --init --recursive
```
```
