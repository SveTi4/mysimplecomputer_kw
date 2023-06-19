#!/bin/bash

enter_alt_charset_mode="\033(0"
exit_alt_charset_mode="\033(B"
acs_chars="``aaffgghhiijjkkllmmnnooppqqrrssttuuvvwwxxyyzz{{||}}~~"

clear
tput cup 0 6

echo -e "${enter_alt_charset_mode} lqqqqqqqqqk${exit_alt_charset_mode}"

for i in {1..4}; do
tput cup $((1+i)) 7 # Set cursor position for this row

echo -e "${enter_alt_charset_mode}x${acs_chars:0:2}x${exit_alt_charset_mode}   ${enter_alt_charset_mode}x${acs_chars:0:2}x${exit_alt_charset_mode}"
done

tput cup 7 6
echo -e "${enter_alt_charset_mode} mqqqqqqqqqj${exit_alt_charset_mode}"

digit=0

tput cup 1 10
echo -e "${enter_alt_charset_mode}${acs_chars:$((digit*2)):2}${acs_chars:$((digit*2)):2}${acs_chars:$((digit*2)):2}${exit_alt_charset_mode}"
tput cup 6 10
echo -e "${enter_alt_charset_mode}${acs_chars:$((digit*2)):2}${acs_chars:$((digit*2)):2}${acs_chars:$((digit*2)):2}${exit_alt_charset_mode}"

tput cup 8 8
echo -e "3O.O9.2OO3\n"

read

