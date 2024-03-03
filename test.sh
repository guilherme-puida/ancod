#!/usr/bin/env bash

passes=0
fails=0

escape() {
  local esc=${1@Q}
  echo "${esc:2:-1}"
}

c() {
  printf '\033[%dm' "$1"
}

reset="$(c 0)"

check() {
  local description="$1"
  local input="$2"
  local expected="$3"

  local actual
  actual="$(./ancod "$input")"

  if [[ "$expected" != "$actual" ]]; then
    ((fails++))
    echo "FAIL $description"
    escape "  : $actual != $expected"
    echo   "  : $actual$reset != $expected$reset"
  else
    ((passes++))
    echo "PASS $description"
  fi
}

check 'bold'   '[b]bold'   "$(c 1)bold"
check 'dim'    '[d]dim'    "$(c 2)dim"
check 'italic' '[i]italic' "$(c 3)italic"
check 'under'  '[u]under'  "$(c 4)under"
check 'strike' '[s]strike' "$(c 9)strike"

echo "$passes passes, $fails fails"
exit "$fails"
