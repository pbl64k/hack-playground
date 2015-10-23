<?hh

    function rec(int $n): void
    {
        print(strval($n)."\n");
        rec($n + 1);
    }

    rec(1);

