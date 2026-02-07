<!-- README.md file -->

# Clock

## build:

```bash
git clone https://github.com/simonryeland/clock.git
cd clock
qmake6 clock.pro
make
```

## clear:

```bash
make clean; rm -r build_tmp/ clock Makefile .qmake.stash
```

<!--
time { file="clock"; make clean; qmake6 "${file}.pro" && make && ./${file}; }

git add . && git commit -m "update all files" && git push origin main

git checkout main
git rebase -i --root
git push origin main --force

grep -rin --include="*.h" --include="*.cpp" "find" .
-->

