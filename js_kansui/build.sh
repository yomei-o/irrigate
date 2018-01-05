CSRCDIR=src
JSRCDIR=out
TOOLDIR=tool
HTMLDIR=html
PACKAGE=com/yomei


FILES="js_test websocket"

mkdir -p $JSRCDIR

for i in $FILES; do
echo $i
g++ -o $JSRCDIR/$i.js -DJAVASCRIPT -E -C -P $CSRCDIR/$i.cpp
#cp -f $JSRCDIR/$i.js /cygdrive/d/yomei/GoogoeDrive/happy-wedding/htdocs2/$i.js
done

cp -f $HTMLDIR/main.html $JSRCDIR/.
cp -f $HTMLDIR/jquery*.js $JSRCDIR/.


echo ------------------------------
echo Build OK!!!!!!!!
echo ------------------------------

