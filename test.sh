#!/usr/bin/env bash

verbose="$1"

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
    if [[ "$verbose" == "1" ]]; then
      escape "  : $actual != $expected"
      echo   "  : $actual$reset != $expected$reset"
    fi
  fi
}

check 'invalid tag' '[abc]hello'    '[abc]hello'
check 'mismatched'  '[[]hello'      '[[]hello'
check 'closing'     ']hello'        ']hello'
check 'nested'      '[[]]hello'     '[[]]hello'
check 'escaped'     '\[b]hello \n'  '[b]hello \n'

check 'bold'   '[b]bold[/b] no'   "$(c 1)bold$(c 22) no"
check 'dim'    '[d]dim[/d] no'    "$(c 2)dim$(c 22) no"
check 'italic' '[i]italic[/i] no' "$(c 3)italic$(c 23) no"
check 'under'  '[u]under[/u] no'  "$(c 4)under$(c 24) no"
check 'strike' '[s]strike[/s] no' "$(c 9)strike$(c 29) no"

echo "$passes passes, $fails fails"
exit "$fails"
