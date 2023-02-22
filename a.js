const CT = new Set();
const ERROR = new Set();
const { ChildProcess } = require("child_process");

const _REG_TYPE = (t) =>
    `(?<${t}>(?:void|int|char|short|long|float|double\\s*\\*?)`,
  REG_TYPE = (t) => `(?<${t}>[a-zA-Z_]+\\s*\\*?)`,
  REG_NAME = (name) => `(?<${name}>[a-zA-Z_]+)`;

const regex = new RegExp(
    `^\\s*(?<sign>unsigned|signed)?\\s*${REG_TYPE("returnType")}${REG_NAME(
      "funcName"
    )}\\s*\\((?<args>[^)]*)\\s*\\)\\s*;$`
  ),
  regName = /^lv_(?<obj>\w+?)_(?<action>\w+?)_(?<attr>.+)$/,
  //regArg = new RegExp(`^\\s*${REG_TYPE("type")}${REG_NAME("name")}\\s*$`);
  regArg =
    /(?<type>(?:struct\s*(?<struct>[^ ]+))?.+)(?<= )(?<name>[a-zA-Z_]+$)/;

//const toHump = (v) => [v[0].toUpperCase(), ...v.slice(1)].join(""),
const toHump = (v) => {
    const r = v
      .trim()
      .replace(/^_?/, "_")
      .replaceAll(/_(\w)/g, (_, v) => v.toUpperCase());
    return r;
  },
  transName = (name) => {
    const {
      groups: { obj, action, attr },
    } = regName.exec(name);

    //return [action, attr, "For", obj].map(toHump).join("");
    return [attr, "For", obj].map(toHump).join("");
  };

const transRt = (rt) => {
  return (
    {
      void: ["", ""],
      bool: ["bool", "res"],
      "chat *": ["string", "C.GoString(res)"],
    }[rt] ?? ["", ""]
  );
};

const addCt = (t) => {
  if (t.startsWith("lv_") || t.startsWith("_")) {
    CT.add(t.trim());
  }
};

const transTypeForGo = (t) => {
  const r =
    {
      "char *": "string",
      "void *": "any",
      int8_t: "int8",
      uint8_t: "uint8",
      int16_t: "int16",
      uint16_t: "uint16",
      int32_t: "int32",
      uint32_t: "uint32",
      int64_t: "C.int64",
      uint64_t: "C.uint64",
      bool: "bool",
    }[t] ?? `lib.${toHump(t)}`;

  addCt(t);

  if (r.includes("*")) {
    return `*${r.replace("*", "")}`;
  }
  return r;
};

const transTypeForC = (t, name) => {
  const r =
    {
      "char *": `CString`,
      "void *": `unsafe.Pointer(&${name})`,
      int16_t: `short`,
      uint16_t: `ushort`,
      int32_t: "int",
      uint32_t: "uint",
      int64_t: "long",
      uint64_t: "ulong",
    }[t] ?? t;

  addCt(r);

  if (r.includes(")")) {
    return r;
  }

  if (r.includes("*")) {
    return `(*C.${r.replace("*", "").trim()})(unsafe.Pointer(${name}))`;
  }

  return `C.${r}(${name})`;
};

const transArg = (args) => {
  const c = [],
    go = [];

  args
    .split(",")
    .map((a) =>
      regArg
        .exec(a.trim())
        .slice(1, 4)
        .map((v) => (v ?? "").trim())
    )
    .forEach(([type, struct, name]) => {
      go.push(`${name} ${transTypeForGo(struct ? `${struct} *` : type, name)}`);

      c.push(
        struct
          ? `(*C.struct__${struct.replace(/^_/, "")})(unsafe.Pointer(${name}))`
          : transTypeForC(type, name)
      );
    });

  return [c, go].map((v) => v.join(","));
};

const RETURN_TYPE = "丨_RETURN_TYPE__",
  NAME = "丨_NAME__",
  NAME_C = "丨_NAME__C_|",
  ARGS = "丨_ARGS_GO_|",
  ARGS_C = "丨_ARGS__C_|",
  RETURN_VAL = "丨_RETURN_VAL__",
  RETURN_TAG = "RETURN_TAG",
  RETURN_VAR = "res := ";

const TEMPLATE = `func ${NAME}(${ARGS}) ${RETURN_TYPE} {
  ${RETURN_TAG} C.${NAME_C}(${ARGS_C})

  ${RETURN_VAL}
}`;

// cs := C.CString("Hello from stdio")
// C.myprint(cs)
// C.free(unsafe.Pointer(cs))

const doIt = (str) => {
  const {
    groups: { returnType, funcName, args },
  } = regex.exec(str);

  const [rt, rv] = transRt(returnType),
    [argC, argGo] = transArg(args);

  let ret = TEMPLATE.replace(NAME_C, funcName)
    .replace(NAME, transName(funcName))
    .replace(RETURN_TYPE, rt)
    .replace(RETURN_VAL, rv ? `return ${rv}` : "")
    .replace(RETURN_TAG, rv ? RETURN_VAR : "")
    .replace(ARGS_C, argC)
    .replace(ARGS, argGo);

  return ret;
};

// const fs = require("fs");
// fs.writeFileSync("./done1.go", fs.readFileSync("ccc1", {encoding:"utf8"}).trim().replaceAll(/,\n\s+/mg, ',').split("\n").map(line => doIt(line)).join("\n"), "utf8");

{
  const done = new Map(),
    fs = require("fs"),
    child_process = require("child_process"),
    regFileName = /lv_(?<obj>\w+?)_(?<action>\w+?)/,
    headerTxt = `package set

/*
#cgo CFLAGS: -I../include/
#cgo LDFLAGS: -L../lib -llvgl
#include "lv_init.h"
*/
import "C"
import (
	"unsafe"
  lib "lvgl-go/src/lib"
)\n`;

  fs.readFileSync("ccc", { encoding: "utf8" })
    .trim()
    .replaceAll(/,\n\s+/gm, ",")
    .replaceAll(/const /g, "")
    .split("\n")
    .forEach((line) => {
      try {
        const {
            groups: { action, obj },
          } = regFileName.exec(line),
          fileName = `${obj}`,
          j = done.get(fileName) ?? done.set(fileName, new Set()).get(fileName);

        //console.log(line);

        j.add(doIt(line));
      } catch (error) {
        ERROR.add(line);
      }
    });

  child_process.execSync("rm -rf ./done_go/* && mkdir -p ./done_go/set");

  done.forEach((v, fileName) => {
    fs.writeFileSync(
      `../go_lvgl/src/set/${fileName}.go`,
      headerTxt + [...v].join("\n"),
      "utf8"
    );
  });

  console.log(
    [...CT]
      .map((v) => {
        let x = v.replace("*", "").replace(/^_/, "");

        return `type ${toHump(x)} C.${x}`;
      })
      .join("\n")
  );

  console.log("-==========================---");
  console.log([...ERROR]);
}
