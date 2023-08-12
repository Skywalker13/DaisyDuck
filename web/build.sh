#!/bin/sh

LANGUAGES="en fr"
BASE=base
INDEX=INDEX.all

mkdir -p ../docs

echo "proceed to base files copy (support for languages: $LANGUAGES)"
cp *.css ../docs/
cp *.png ../docs/
cp $BASE ../docs/index.html
sed -i "s#accesskey=\"[0-9]*\"##g" ../docs/index.html
INC=$(cat $INDEX | tr "\n" " " | sed "s,\&,\\\&,g")
sed -i "s#@@INCLUDE:PAGE@@#$INC#" ../docs/index.html
sed -i "s#@@LANG@@#all#g" ../docs/index.html
sed -i "s#\(id=\"contents\"\)#\1 style=\"visibility: hidden\"#" ../docs/index.html

. ./lang.all

for it in $lang; do
  sed -i "s#__${it}__#$(eval echo \$__${it}__)#g" ../docs/index.html
done


for LANG in $LANGUAGES; do
  . ./lang.$LANG

  for it in $lang; do
    [ ! -f $it.$LANG ] && continue

    echo "build ($LANG) page $it"

    cp $BASE ../docs/$it.$LANG.html
    INC=$(cat $it.$LANG | tr "\n" " " | sed "s,\&,\\\&,g")
    sed -i "s#@@INCLUDE:PAGE@@#$INC#" ../docs/$it.$LANG.html
    sed -i "s#\(@@INCLUDE:[^ ]*@@\)#\1\n#g" ../docs/$it.$LANG.html

    for inc in `grep "@@INCLUDE:.*@@" ../docs/$it.$LANG.html | sed "s#.*@@INCLUDE:\(.*\)@@.*#\1#g"`; do
      INC=$(cat $inc | tr "\n" " " | sed "s,\&,\\\&,g")
      sed -i "s#@@INCLUDE:$inc@@#$INC#" ../docs/$it.$LANG.html
    done

    sed -i "s#@@LANG@@#$LANG#g" ../docs/$it.$LANG.html

    for it2 in $lang; do
      sed -i "s#__${it2}__#$(eval echo \$__${it2}__)#g" ../docs/$it.$LANG.html
      sed -i "s#@@$it2@@#$it2#g" ../docs/$it.$LANG.html
    done
  done
done

echo "finished"
