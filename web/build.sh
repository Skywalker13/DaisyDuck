#!/bin/sh

LANGUAGES="en fr"
BASE=base
INDEX=INDEX.all

mkdir -p site

echo "proceed to base files copy (support for languages: $LANGUAGES)"
cp *.css site/
cp *.png site/
cp $BASE site/index.html
sed -i "s#accesskey=\"[0-9]*\"##g" site/index.html
INC=$(cat $INDEX | tr "\n" " " | sed "s,\&,\\\&,g")
sed -i "s#@@INCLUDE:PAGE@@#$INC#" site/index.html
sed -i "s#@@LANG@@#all#g" site/index.html
sed -i "s#\(id=\"contents\"\)#\1 style=\"visibility: hidden\"#" site/index.html

. ./lang.all

for it in $lang; do
  sed -i "s#__${it}__#$(eval echo \$__${it}__)#g" site/index.html
done


for LANG in $LANGUAGES; do
  . ./lang.$LANG

  for it in $lang; do
    [ ! -f $it.$LANG ] && continue

    echo "build ($LANG) page $it"

    cp $BASE site/$it.$LANG.html
    INC=$(cat $it.$LANG | tr "\n" " " | sed "s,\&,\\\&,g")
    sed -i "s#@@INCLUDE:PAGE@@#$INC#" site/$it.$LANG.html
    sed -i "s#\(@@INCLUDE:[^ ]*@@\)#\1\n#g" site/$it.$LANG.html

    for inc in `grep "@@INCLUDE:.*@@" site/$it.$LANG.html | sed "s#.*@@INCLUDE:\(.*\)@@.*#\1#g"`; do
      INC=$(cat $inc | tr "\n" " " | sed "s,\&,\\\&,g")
      sed -i "s#@@INCLUDE:$inc@@#$INC#" site/$it.$LANG.html
    done

    sed -i "s#@@LANG@@#$LANG#g" site/$it.$LANG.html

    for it2 in $lang; do
      sed -i "s#__${it2}__#$(eval echo \$__${it2}__)#g" site/$it.$LANG.html
      sed -i "s#@@$it2@@#$it2#g" site/$it.$LANG.html
    done
  done
done

echo "finished"
