#!/usr/bin/python2.7
import os, sys

SRC_DIR     = "src"
BIN_DIR     = "bin"
BIN_NAME    = "json"

INCL_PATH   = ["~/usr/include/", "~/usr/include/x86_64-linux-gnu/"]
LIB_PATH    = ["~/lib/", "~/usr/lib/", "~/usr/lib/x86_64-linux-gnu/"]

RFLAGS      = ["-O3", "-Wall", "-Wextra", "-Werror", "-Wformat"]
DFLAGS      = ["-g", "-Wall", "-Wextra", "-Werror", "-Wformat",
                "-Wunreachable-code", "-Winline", "-fstack-protector"]
DLIBS       = ["m", "dl"]

def str_fmt(fmt, var):
    for k in var:
        fmt = fmt.replace("{%s}" % str(k), var[k])
    return fmt

def ls_dir(path):
    return [os.path.join(dp, f)
            for dp, dn, fn in
                os.walk(path) for f in fn]

def main():
    os.chdir(sys.path[0])

    compiler = "gcc"
    flags = RFLAGS

    for flag in sys.argv:
        if flag == "-gcc":
            compiler = "gcc"
        elif flag == "-clang":
            compiler = "clang"
        elif flag == "-d":
            flags = DFLAGS

    if not os.path.exists(BIN_DIR):
        os.makedirs(BIN_DIR)

    cfiles = filter(lambda x:x.endswith((".c", ".C")), ls_dir(SRC_DIR))
    cmd = str_fmt(
        "{compiler} {flags} -o {outfile} {srcfiles} {ipath} {lpath} {libs}",
        {
            "compiler"  : compiler,
            "flags"     : " ".join(flags),
            "outfile"   : BIN_DIR + "/" + BIN_NAME,
            "srcfiles"  : " ".join(cfiles),
            "ipath"     : " ".join(map(lambda x: "-I " + x, INCL_PATH)),
            "lpath"     : " ".join(map(lambda x: "-L " + x, LIB_PATH)),
            "libs"      : " ".join(map(lambda x: "-l " + x, DLIBS)),
        })
    print("compiling...")
    os.system(cmd)
    print("done!")

if __name__ == "__main__":
    main()

