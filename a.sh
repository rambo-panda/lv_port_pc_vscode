#!/bin/bash

sources=`find ./ -name "*.h" -type f -not -path '*/ccc*' -not -path '*/demos/*' -not -path '*/examples/*' -not -path '*/test/*' -not -path '*/lv_examples/*'`

# grep  _set_ -A 2 /Users/rambo/work_space/lvgl_tutorial/lv_sim_vscode_sdl_8_3/lvgl/src/core/lv_obj_style.c > ccc

echo "" > ccc

for file in ${sources}
do
  echo "$file"
  grep "_set_" -A 1 ${file} >> ccc
done

gsed -i '/^static/d' ccc
gsed -i '/^#/d' ccc
gsed -i '/^ \*/d' ccc
gsed -i '/^\//d' ccc
gsed -i '/^--/d' ccc
gsed -i '/^[  ]*$/d' ccc
gsed -i '/^  lv_/d' ccc
gsed -i '/^{/d' ccc
gsed -i '/^}/d' ccc
gsed -i '/^    lv/d' ccc
