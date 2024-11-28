#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
FLAGS="b e n s t v"
VIOLET="$(tput setaf 93)"
PINK="$(tput setaf 201)"
BASE="$(tput setaf 7)"

tests=(
    "VAR tests/Breaking_The_Habit.txt"
    "tests/Numb.txt"
    "-n tests/Popular_Monster.txt tests/Lose_Yourself.txt"
)

testing() {
    t=$(echo $@ | sed "s/VAR/$var/")
    ./s21_cat $t > s21_tests.log
    cat $t > tests.log
    DIFF_RES="$(diff -s s21_tests.log tests.log)"
    RESULT="SUCCESS"
    (( COUNTER++ ))
    if [ "$DIFF_RES" == "Files s21_tests.log and tests.log are identical" ]
    then
      (( SUCCESS++ ))
        RESULT="SUCCESS"
    else
      (( FAIL++ ))
        RESULT="FAIL"
    fi
        echo "------------------------------------------------------------------------------------"
    echo "|${VIOLET}${SUCCESS}${BASE}|${PINK}${FAIL}${BASE}| ${VIOLET}${RESULT}${BASE} cat $t"

    if [ "$RESULT" == "FAIL" ]
    then
        echo "------------------------------------------------------------------------------------"
        echo "|${VIOLET}${SUCCESS}${BASE}|${PINK}${FAIL}${BASE}| ${PINK}${RESULT}${BASE} cat $t"
echo "cat $t" >> fail.log
    fi

    rm s21_tests.log tests.log
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

for var1 in $FLAGS
do
    for var2 in $FLAGS
    do
        for var3 in $FLAGS
        do
            for var4 in $FLAGS
            do
                if [ $var1 != $var2 ] && [ $var2 != $var3 ] \
                && [ $var1 != $var3 ] && [ $var1 != $var4 ] \
                && [ $var2 != $var4 ] && [ $var3 != $var4 ]
                then
                    for i in "${tests[@]}"
                    do
                        var="-$var1 -$var2 -$var3 -$var4"
                        testing $i
                    done
                fi
            done
        done
    done
done

echo "${PINK}FAIL: ${FAIL}${BASE}"
echo "${VIOLET}SUCCESS: ${SUCCESS}${BASE}"
echo "ALL: $COUNTER"
