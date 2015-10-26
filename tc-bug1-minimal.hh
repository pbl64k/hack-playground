<?hh // strict

    interface ITest
    {
    }

    final class Dummy
    {
        final public function emit(): void
        {
            print('Test.'."\n");
        }
    }

    final class Unsafe<Tx> implements ITest
    {
        final public function __construct(private Tx $x)
        {
        }

        // the culprit
        final public static function convert(ITest $x): Unsafe<Tx>
        {
            invariant($x instanceof Unsafe, '...or else');
            return $x;
        }

        final public static function mkInt(): Unsafe<int>
        {
            return new self(1);
        }

        final public static function emit(Unsafe<Dummy> $x): void
        {
            $x->x->emit();
        }

        final public static function test(): void
        {
            Unsafe::emit(Unsafe::convert(Unsafe::mkInt()));
        }
    }

