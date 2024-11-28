#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
RESULT=0
FLAGS="v c l n h o"
VIOLET="$(tput setaf 93)"
PINK="$(tput setaf 201)"
BASE="$(tput setaf 7)"

tests=(
    "habit tests/Breaking_The_Habit.txt VAR"
    "for s21_grep.c VAR"
    "VAR -e for -e ^int s21_grep.c"
    "-e regex -e ^print s21_grep.c VAR -f tests/Patterns.txt"
)

more_tests=(
    "-n Verse tests/Lose_Yourself.txt tests/Numb.txt"
    "-n popular tests/Popular_Monster.txt"
    "-n -e ^\. tests/The_Night.txt"
    "-ce ^lose tests/Lose_Yourself.txt tests/Numb.txt"
    "-ne = -e collapse tests/Till_I_Collapse.txt"
    "-e ^in tests/Breaking_The_Habit.txt"
    "-nivh s tests/Lose_Yourself.txt tests/Numb.txt"
    "-ie DEAR tests/Prequel.txt"
    "-enumb tests/Lose_Yourself.txt tests/Numb.txt"
    "-ne = -e numb tests/Numb.txt"
    "-iv in tests/Popular_Monster.txt"
    "-in in tests/The_Night.txt"
    "-c -l cyrcry tests/The_Night.txt tests/Till_I_Collapse.txt"
    "-v tests/Breaking_The_Habit.txt -e the"
    "-in -l in tests/Lose_Yourself.txt"
    "-noe '.' tests/The_Night.txt"
    "-l for tests/Lose_Yourself.txt tests/Numb.txt"
    "-o -e in tests/Prequel.txt"
    "-v tests/Popular_Monster.txt -e the"
    "-e = -e diary tests/Prequel.txt"
    "-noe ing -e as -e the -e not -e is tests/Breaking_The_Habit.txt"
    "-e ing -e as -e the -e not -e is tests/The_Night.txt"
    "-l for tests/The_Night.txt tests/Till_I_Collapse.txt"
    "-f tests/Breaking_The_Habit.txt tests/Numb.txt"
    " "
    "-+-=+"
    "-n -t -s -u"
)

testing()
{
    OS=$(uname -s)
    t=$(echo $@ | sed "s/VAR/$var/")
    if [[ $OS == "Darwin" ]]
    then
        leaks -atExit -- ./s21_grep $t > test_leak.log
        leak=$(grep -o "0 leaks for 0 total leaked bytes." test_leak.log)
        RESULT="SUCCESS"
        (( COUNTER++ ))
        if [[ $leak == "0 leaks for 0 total leaked bytes." ]]
        then
        (( SUCCESS++ ))
            RESULT="SUCCESS"
            echo "------------------------------------------------------------------------------------"
            echo "|${VIOLET}${SUCCESS}${BASE}|${PINK}${FAIL}${BASE}| ${VIOLET}${RESULT}${BASE} grep $t"
        else
        (( FAIL++ ))
            RESULT="FAIL"
            echo "------------------------------------------------------------------------------------"
            echo "|${VIOLET}${SUCCESS}${BASE}|${PINK}${FAIL}${BASE}| ${VIOLET}${RESULT}${BASE} grep $t"
            echo "grep $t" >> fail_leak.log
        fi
    elif [[ $OS == *"Linux"* ]]
    then
        result=$(valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=yes -s ./s21_grep $t 2>&1)
        echo $result > test_leak.log
        leak=$(grep -o "ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)" test_leak.log)
        RESULT="SUCCESS"
        (( COUNTER++ ))
        if [[ $leak == "ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)" ]]
        then
        (( SUCCESS++ ))
            RESULT="SUCCESS"
            echo "------------------------------------------------------------------------------------"
            echo "|${VIOLET}${SUCCESS}${BASE}|${PINK}${FAIL}${BASE}| ${VIOLET}${RESULT}${BASE} grep $t"
        else
        (( FAIL++ ))
            RESULT="FAIL"
            echo "------------------------------------------------------------------------------------"
            echo "|${VIOLET}${SUCCESS}${BASE}|${PINK}${FAIL}${BASE}| ${VIOLET}${RESULT}${BASE} grep $t"
            echo "valgrind grep $t" >> fail_leak.log
        fi
    fi
    rm test_leak.log
}

for i in "${more_tests[@]}"
do
    testing $i
done

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
        if [ $var1 != $var2 ]
        then
            for i in "${tests[@]}"
            do
                var="-$var1$var2"
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
                    var="-$var1$var2$var3"
                    testing $i
                done
            fi
        done
    done
done

echo "${PINK}FAIL: ${FAIL}${BASE}"
echo "${VIOLET}SUCCESS: ${SUCCESS}${BASE}"
echo "ALL: $COUNTER"
