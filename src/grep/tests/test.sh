#!/bin/bash

SUCCESS=0
FAIL=0
COUNTER=0
FLAGS="v c l n h o"
VIOLET="$(tput setaf 93)"
PINK="$(tput setaf 201)"
BASE="$(tput setaf 7)"

tests=(
    "VAR for s21_grep.c"
    "VAR the tests/Breaking_The_Habit.txt tests/Prequel.txt"
    "the tests/Breaking_The_Habit.txt tests/Numb.txt tests/Lose_Yourself.txt VAR"
    "for s21_grep.c VAR"
    "VAR -e numb -e ^the tests/Numb.txt"
    "-f tests/Patterns.txt s21_grep.c"
    "-e regex -e ^print VAR -f tests/Patterns.txt s21_grep.c"
    "for -e while -e void s21_grep.c VAR -f tests/Patterns.txt"
    "VAR -enumb -efor s21_grep.c tests/Numb.txt"
    "VAR -noe ing -e as -e the -e not -e is tests/Breaking_The_Habit.txt"
    "VAR -ce ^lose tests/Lose_Yourself.txt tests/Numb.txt"
)

more_tests=(
    "-n Verse tests/Lose_Yourself.txt tests/Numb.txt"
    "-n popular tests/Popular_Monster.txt"
    "-n -e ^\. tests/The_Night.txt"
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
    "-e ing -e as -e the -e not -e is tests/The_Night.txt"
    "-l for tests/The_Night.txt tests/Till_I_Collapse.txt"
    "-f tests/Breaking_The_Habit.txt tests/Numb.txt"
    " "
    "-+-=+"
    "-n -t -s -u"
)


testing()
{
    t=$(echo $@ | sed "s/VAR/$var/")
    ./s21_grep $t > test_s21_grep.log
    grep $t > test_grep.log
    DIFF_RES="$(diff -s test_s21_grep.log test_grep.log)"
    RESULT="SUCCESS"
    (( COUNTER++ ))
    if [ "$DIFF_RES" == "Files test_s21_grep.log and test_grep.log are identical" ]
    then
      (( SUCCESS++ ))
        RESULT="SUCCESS"
    else
      (( FAIL++ ))
        RESULT="FAIL"
    fi
        echo "------------------------------------------------------------------------------------"
        echo "|${VIOLET}${SUCCESS}${BASE}|${PINK}${FAIL}${BASE}| ${VIOLET}${RESULT}${BASE} grep $t"

    if [ "$RESULT" == "FAIL" ]
    then
        echo "------------------------------------------------------------------------------------"
        echo "|${VIOLET}${SUCCESS}${BASE}|${PINK}${FAIL}${BASE}| ${PINK}${RESULT}${BASE} grep $t"
echo "grep $t" >> fail.log
    fi

    rm test_s21_grep.log test_grep.log
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
