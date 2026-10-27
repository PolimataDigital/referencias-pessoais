#!/usr/bin/env python3
"""Compila e executa exemplos com GCC/Clang, sem dependencias Python externas.

MSVC: use o CMakeLists.txt, pois as flags desta rotina sao GCC/Clang.
Resultados de cada execucao sao impressos; falha interrompe o processo.
"""
import argparse
import os
from pathlib import Path
import shlex
import subprocess


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cc", default=os.environ.get("CC", "gcc"))
    parser.add_argument("--cxx", default=os.environ.get("CXX", "g++"))
    parser.add_argument("--sanitize", choices=["address,undefined", "undefined", "thread"])
    parser.add_argument("--release", action="store_true")
    args = parser.parse_args()
    raiz = Path(__file__).resolve().parent
    sufixo = args.sanitize or ("release" if args.release else "debug")
    destino = raiz.parents[1] / "build" / ("c-cpp-" + sufixo.replace(",", "-"))
    destino.mkdir(parents=True, exist_ok=True)
    flags = ["-Wall", "-Wextra", "-Wpedantic", "-Wconversion", "-Wshadow", "-Werror", "-g"]
    flags += ["-O2", "-DNDEBUG"] if args.release else ["-O0"]
    if args.sanitize:
        flags += ["-fsanitize=" + args.sanitize, "-fno-omit-frame-pointer"]
    cc, cxx = shlex.split(args.cc), shlex.split(args.cxx)
    for compilador in (cc, cxx):
        print(subprocess.check_output(compilador + ["--version"], text=True).splitlines()[0], flush=True)
    executaveis = []
    for linguagem, compilador, padrao, extensao in [
        ("c", cc, "c17", "c"), ("cpp", cxx, "c++20", "cpp")
    ]:
        for fonte in sorted((raiz / linguagem).glob("*." + extensao)):
            if args.sanitize == "thread" and fonte.stem != "concorrencia":
                continue
            alvo = destino / (linguagem + "_" + fonte.stem)
            extras = ["-pthread"] if fonte.stem == "concorrencia" else []
            subprocess.run(compilador + ["-std=" + padrao] + flags + extras +
                           [str(fonte), "-o", str(alvo)], check=True)
            executaveis.append(alvo)
    if args.sanitize != "thread":
        objeto = destino / "api.o"
        subprocess.run(cc + ["-std=c17"] + flags + ["-c", str(raiz / "ffi/api.c"), "-o", str(objeto)], check=True)
        alvo = destino / "ffi_exemplo"
        subprocess.run(cxx + ["-std=c++20"] + flags +
                       [str(raiz / "ffi/main.cpp"), str(objeto), "-o", str(alvo)], check=True)
        executaveis.append(alvo)
    for alvo in executaveis:
        print(alvo.name, flush=True)
        subprocess.run([str(alvo)], check=True)
    print(f"SUCESSO: {len(executaveis)} executaveis", flush=True)


if __name__ == "__main__":
    main()

