const ACTION_TYPE = process.argv[2] ?? "set";

const CT = new Set();
const ERROR = new Set();

const REG_TYPE = (t) => `(?<${t}>[0-9a-zA-Z_]+\\s*\\*?\\s*)`,
  REG_NAME = (name) => `(?<${name}>[0-9a-zA-Z_]+)`;

const regex = new RegExp(
    `^\\s*(?<sign>unsigned|signed)?\\s*${REG_TYPE("returnType")}${REG_NAME(
      "funcName"
    )}\\s*\\((?<args>[^)]*)\\s*\\)\\s*;$`
  ),
  regName = /^lv_(?<obj>\w+?)_(?<action>\w+?)_(?<attr>.+)$/,
  // regArg = new RegExp(`^\\s*${REG_TYPE("type")}${REG_NAME("name")}\\s*$`);
  regArg =
    /(?<type>(?:struct\s*(?<struct>[^ ]+))?.+)(?<= )(?<name>[a-zA-Z_]+$)/;

// const toHump = (v) => [v[0].toUpperCase(), ...v.slice(1)].join(""),
const toHump = (v) => {
    const r = v
      .trim()
      .replace(/^_?/, "_")
      .replaceAll(/_(\w)/g, (_, k) => k.toUpperCase());
    return r;
  },
  transName = (name) => {
    const {
      groups: { action, attr },
    } = regName.exec(name);

    return [attr].map(toHump).join("");
  };

const transRt = (rt) => {
  const rt2 = rt.replace("*", "").trim(),
    k = rt.trim() === rt2;

  return (
    {
      void: ["", ""],
      bool: ["bool", "bool(res)"],
      char: ["string", "C.GoString(res)"],
      lv_coord_t: ["int16", "int16(res)"],
      uint32_t: ["uint32", "uint32(res)"],
    }[rt2] ?? [
      `${k ? "" : "*"}types.${toHump(rt2)}`,
      k
        ? `types.${toHump(rt2)}(res)`
        : `(*types.${toHump(rt2)})(unsafe.Pointer(res))`,
    ]
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
      if (name === "type") {
        name = "_type"; // go关键字
      }
      if (type === "lv_obj_t *") {
        name = `${ACTION_TYPE}ter.CStructLvObjT`;
        // c.push(`${name} := ${transTypeForC(type, "${ACTION_TYPE}ter.cObj")}`);
        // return ;
        c.push(name);
        return;
      }
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
  OBJ_TAG = "丨PARNET_TAG|",
  RETURN_VAL = "丨_RETURN_VAL__",
  RETURN_TAG = "丨RETURN_TAG__",
  RETURN_VAR = "res := ";

const TEMPLATE = `func ${OBJ_TAG} ${NAME}(${ARGS}) ${RETURN_TYPE} {
  ${RETURN_TAG} C.${NAME_C}(${ARGS_C})

  ${RETURN_VAL}
}`;

// cs := C.CString("Hello from stdio")
// C.myprint(cs)
// C.free(unsafe.Pointer(cs))

const doIt = (str, objTag) => {
  const {
    groups: { returnType, funcName, args },
  } = regex.exec(str);

  const [rt, rv] = transRt(returnType),
    [argC, argGo] = transArg(args),
    [rType, rValue, rTag] = rv
      ? [rt, `return ${rv}`, RETURN_VAR]
      : [objTag, `return ${ACTION_TYPE}ter`, ""];

  return TEMPLATE.replace(NAME_C, funcName)
    .replace(NAME, transName(funcName))
    .replace(OBJ_TAG, `(${ACTION_TYPE}ter ${objTag})`)
    .replace(RETURN_TYPE, rType)
    .replace(RETURN_VAL, rValue)
    .replace(RETURN_TAG, rTag)
    .replace(ARGS_C, argC)
    .replace(ARGS, argGo);
};

// const fs = require("fs");
// fs.writeFileSync("./done1.go", fs.readFileSync("ccc1", {encoding:"utf8"}).trim().replaceAll(/,\n\s+/mg, ',').split("\n").map(line => doIt(line)).join("\n"), "utf8");

{
  const done = new Map(),
    fs = require("fs"),
    child_process = require("child_process"),
    regFileName = / lv_(?<obj>\w+?)_(?<action>\w+?)_/,
    headerTxt = `package ${ACTION_TYPE}

/*
#cgo CFLAGS: -I../include/
#cgo LDFLAGS: -L../lib -llvgl
#include "lv_init.h"
*/
import "C"
import (
	"unsafe"
  lib "lvgl-go/src/lib"
  types "lvgl-go/src/types"
)
`;

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

        const objTag = toHump(obj);

        j.add(`type ${objTag} ${ACTION_TYPE}
func Create${toHump(obj)}(o *types.LvObjT) ${objTag} {
	return ${objTag}{
		CStructLvObjT: (*C.struct__lv_obj_t)(unsafe.Pointer(o)),
	}
}
`);
        j.add(doIt(line, objTag));
      } catch (error) {
        ERROR.add(line);
      }
    });

  child_process.execSync(
    `rm -rf ./done_go/${ACTION_TYPE} && mkdir -p ./done_go/${ACTION_TYPE}`
  );

  done.forEach((v, fileName) => {
    const t = headerTxt + [...v].join("\n");
    fs.writeFileSync(
      // `../go_lvgl/src/${ACTION_TYPE}/${fileName}.go`,
      `./done_go/${ACTION_TYPE}/${fileName}.go`,
      t.includes("unsafe.") ? t : t.replace(`"unsafe"`, ""),
      "utf8"
    );
  });

  console.log(
    [...CT]
      .map((v) => {
        const x = v.replace("*", "").replace(/^_/, "");

        return `type ${toHump(x)} C.${x}`;
      })
      .join("\n")
  );

  console.log("-==========================---");
  console.log([...ERROR]);
}

// #!/bin/bash
//
// sources=`find ./ -name "*.h" -type f -not -path '*/ccc*' -not -path '*/demos/*' -not -path '*/examples/*' -not -path '*/test/*' -not -path '*/lv_examples/*'`
//
// # grep  _set_ -A 2 /Users/rambo/work_space/lvgl_tutorial/lv_sim_vscode_sdl_8_3/lvgl/src/core/lv_obj_style.c > ccc
//
// echo "" > ccc
//
// for file in ${sources}
// do
//   echo "$file"
//   grep "_set_" -A 1 ${file} >> ccc
// done
//
// gsed -i '/^static/d' ccc
// gsed -i '/^#/d' ccc
// gsed -i '/^ \*/d' ccc
// gsed -i '/^\//d' ccc
// gsed -i '/^--/d' ccc
// gsed -i '/^[  ]*$/d' ccc
// gsed -i '/^  lv_/d' ccc
// gsed -i '/^{/d' ccc
// gsed -i '/^}/d' ccc
// gsed -i '/^    lv/d' ccc

// ===============
// #!/bin/bash

// sources=`find ./ -name "*.h" -type f -not -path '*/ccc*' -not -path '*/demos/*' -not -path '*/examples/*' -not -path '*/test/*' -not -path '*/lv_examples/*'`

// # grep  _set_ -A 2 /Users/rambo/work_space/lvgl_tutorial/lv_sim_vscode_sdl_8_3/lvgl/src/core/lv_obj_style.c > ccc

// echo "" > ccc

// for file in ${sources}
// do
//   echo "$file"
//   grep "lv_label_get_"  ${file} >> ccc
// done

// gsed -i '/^static/d' ccc
// gsed -i '/^#/d' ccc
// gsed -i '/^ \*/d' ccc
// gsed -i '/^\//d' ccc
// gsed -i '/^--/d' ccc
// gsed -i '/^[  ]*$/d' ccc
// gsed -i '/^  lv_/d' ccc
// gsed -i '/^{/d' ccc
// gsed -i '/^}/d' ccc
// gsed -i '/^    lv/d' ccc
