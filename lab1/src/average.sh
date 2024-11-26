#!/bin/bash


if [ "$#" -eq 0 ]; then
    echo "Нет входных аргументов."
    exit 1
fi

sum=0
count=0

for number in "$@"; do
    if ! [[ "$number" =~ ^-?[0-9]+(\.[0-9]+)?$ ]]; then
        echo "Ошибка: '$number' не является числом."
        # exit 1
    fi
    sum=$(echo "$sum + $number" | bc)
    count=$((count + 1))
done

average=$(echo "scale=2; $sum / $count" | bc)

echo "Количество аргументов: $count"
echo "Среднее арифметическое: $average"