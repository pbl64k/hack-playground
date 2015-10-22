<?hh // strict

	require_once(__DIR__.'/Data/Optional.hh');
	require_once(__DIR__.'/Data/Lazy.hh');
	require_once(__DIR__.'/Data/ConsList.hh');
	
    function printOptional(\Data\Optional<int> $x): void
    {
    	print(
                $x->match(
                        () ==> 'NONE',
                        $x ==> strval($x))
                ."\n");
    }

    function main(): void
    {
	    $m1 = \Data\Optional::none();
	    $m2 = \Data\Optional::some(1);
        $m5 = \Data\Optional::fromNullable(null);
        $m6 = \Data\Optional::fromNullable(2);
	    
        $succ = $x ==> $x + 1;

        $m3 = $m1->fmap($succ);
        $m4 = $m2->fmap($succ);
        $m7 = $m5->fmap($succ);
        $m8 = $m6->fmap($succ);

	    printOptional($m1);
	    printOptional($m2);
	    printOptional($m3);
	    printOptional($m4);
	    printOptional($m5);
	    printOptional($m6);
	    printOptional($m7);
	    printOptional($m8);

        $x = \Data\Lazy::delay(
                function ()
                {
                    print('bork bork bork!'."\n");
                    return 42;
                });

        $y = $x->fmap($succ);

        print($x->force()."\n");
        print($y->force()."\n");
        print($x->force()."\n");

        $xs = \Data\ConsList::lst(1, \Data\ConsList::lst(2, \Data\ConsList::lst(3, \Data\ConsList::nil())));

        print($xs->fold($x ==> $x->match(() ==> 0, $x ==> $x->match(($x, $y) ==> $x + $y)))."\n");
        print($xs->reduce(0, ($x, $y) ==> $x + $y)."\n");
        print($xs->fmap($succ)->reduceLeft(1, ($x, $y) ==> $x * $y)."\n");

        $xs->fmap($succ)->reverse()->fmap($x ==> { print($x."\n"); });
    }
