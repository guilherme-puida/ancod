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

check 'fg black'   '[^black]black'     "$(c 30)black"
check 'fg red'     '[^red]red'         "$(c 31)red"
check 'fg green'   '[^green]green'     "$(c 32)green"
check 'fg yellow'  '[^yellow]yellow'   "$(c 33)yellow"
check 'fg blue'    '[^blue]blue'       "$(c 34)blue"
check 'fg magenta' '[^magenta]magenta' "$(c 35)magenta"
check 'fg cyan'    '[^cyan]cyan'       "$(c 36)cyan"
check 'fg white'   '[^white]white'     "$(c 37)white"

check 'bg black'   '[*black]black'     "$(c 40)black"
check 'bg red'     '[*red]red'         "$(c 41)red"
check 'bg green'   '[*green]green'     "$(c 42)green"
check 'bg yellow'  '[*yellow]yellow'   "$(c 43)yellow"
check 'bg blue'    '[*blue]blue'       "$(c 44)blue"
check 'bg magenta' '[*magenta]magenta' "$(c 45)magenta"
check 'bg cyan'    '[*cyan]cyan'       "$(c 46)cyan"
check 'bg white'   '[*white]white'     "$(c 47)white"

echo "$passes passes, $fails fails"
exit "$fails"
