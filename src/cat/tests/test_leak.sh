#!/bin/bash


SUCCESS=0
FAIL=0
COUNTER=0
RESULT=0
FLAGS="b e n s t v"
VIOLET="$(tput setaf 93)"
PINK="$(tput setaf 201)"
BASE="$(tput setaf 7)"

tests=(
    "VAR tests/Breaking_The_Habit.txt"
    "tests/Numb.txt"
    "-n -b tests/Lose_Yourself.txt"
    "-s -n -e tests/Popular_Monster.txt"
    "-n tests/Popular_Monster.txt tests/Lose_Yourself.txt"
    "-n -t int"
    "-s -o int file.txt"
    "-n tests/Breaking_The_Habit.txt"
    "cat -n cat -t"
    "."
    ">->->-"
)

testing()
{
    OS=$(uname -s)
    t=$(echo $@ | sed "s/VAR/$var/")
    if [[ "$OS" == "Darwin" ]]
    then
        leaks -atExit -- ./s21_cat $t > test_leak.log
        leak=$(grep -o "0 leaks for 0 total leaked bytes." test_leak.log)
        RESULT="SUCCESS"
        (( COUNTER++ ))
        if [[ "$leak" == *"0 leaks for 0 total leaked bytes."* ]]
        then
        (( SUCCESS++ ))
            RESULT="SUCCESS"
echo "------------------------------------------------------------------------------------"
            echo "|${VIOLET}${SUCCESS}${BASE}|${PINK}${FAIL}${BASE}| ${VIOLET}${RESULT}${BASE} cat $t"
        else
        (( FAIL++ ))
            RESULT="FAIL"
echo "------------------------------------------------------------------------------------"
            echo "|${VIOLET}${SUCCESS}${BASE}|${PINK}${FAIL}${BASE}| ${PINK}${RESULT}${BASE} cat $t"
            echo "leaks cat $t" >> fail_leak.log
        fi
    elif [[ "$OS" == "Linux" ]]
    then
        result=$(valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes -s ./s21_cat $t 2>&1)
        echo $result > test_leak.log
        leak=$(grep -o "ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)" test_leak.log)
        RESULT="SUCCESS"
        (( COUNTER++ ))
        if [[ "$leak" == "ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)" ]]
        then
        (( SUCCESS++ ))
            RESULT="SUCCESS"
echo "------------------------------------------------------------------------------------"
            echo "|${VIOLET}${SUCCESS}${BASE}|${PINK}${FAIL}${BASE}| ${VIOLET}${RESULT}${BASE} cat $t"
        else
        (( FAIL++ ))
            RESULT="FAIL"
echo "------------------------------------------------------------------------------------"
            echo "|${VIOLET}${SUCCESS}${BASE}|${PINK}${FAIL}${BASE}| ${PINK}${RESULT}${BASE} cat $t"
            echo "valgrind cat $t" >> fail_leak.log
        fi
    fi
    rm test_leak.log
}

for var1 in $FLAGS
do
    for i in "${tests[@]}"
    do
        var="-$var1"
        testing $i
    done
done

for var1 in $FLAGS
do
    for var2 in $FLAGS
    do
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1 -$var2"
                testing $i
            done
        fi
    done
done

for var1 in $FLAGS
do
    for var2 in $FLAGS
    do
        for var3 in $FLAGS
        do
            if [ $var1 != $var2 ] && [ $var2 != $var3 ] && [ $var1 != $var3 ]
            then
                for i in "${tests[@]}"
                do
                    var="-$var1 -$var2 -$var3"
                    testing $i
                done
            fi
        done
    done
done

echo "${PINK}$FAIL: ${FAIL}${BASE}"
echo "${VIOLET}SUCCESS: ${SUCCESS}${BASE}"
echo "ALL: $COUNTER"
